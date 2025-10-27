#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/toriRenderingSystem.hpp>
#include <CAD_modeler/model/systems/nameSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/c0CurveSystem.hpp>
#include <CAD_modeler/model/systems/c2CurveSystem.hpp>
#include <CAD_modeler/model/systems/interpolationCurveSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesSystem.hpp>

#include <CAD_modeler/model/components/drawStd.hpp>

#include <fstream>
#include <unordered_set>


Position ParsePosition(const auto& json)
{
    const float x = json["position"]["x"];
    const float y = json["position"]["y"];
    const float z = json["position"]["z"];

    return {x, y, z};
}


Rotation ParseRotation(const auto& json)
{
    const float x = json["rotation"]["x"];
    const float y = json["rotation"]["y"];
    const float z = json["rotation"]["z"];

    return {x, y, z};
}


Scale ParseScale(const auto& json)
{
    const float x = json["scale"]["x"];
    const float y = json["scale"]["y"];
    const float z = json["scale"]["z"];

    return {x, y, z};
}


void LoadManager::Load(const std::string& path, Coordinator &coordinator, ToLoad options)
{
    std::ifstream f(path);
    json data = json::parse(f);

    PointsToParse pointsToParse(data, options);

    LoadPoints(data, coordinator, pointsToParse);

    for (auto& geometry: data["geometry"]) {
        std::string objectType = geometry["objectType"];

        if (objectType == "torus" && options & Tori)
            LoadTori(geometry, coordinator);
        else if (objectType == "bezierC0" && options & C0Curves)
            LoadC0Curves(geometry, coordinator);
        else if (objectType == "bezierC2" && options & C2Curves)
            LoadC2Curves(geometry, coordinator);
        else if (objectType == "interpolatedC2" && options & InterpolationCurves)
            LoadInterpolationCurves(geometry, coordinator);
        else if (objectType == "bezierSurfaceC0" && options & C0Surfaces)
            LoadC0Patches(geometry, coordinator);
        else if (objectType == "bezierSurfaceC2" && options & C2Surfaces)
            LoadC2Patches(geometry, coordinator);
    }
}

LoadManager::PointsToParse::PointsToParse(json &data, const ToLoad options)
{
    const int pointsCnt = data["points"].size();

    if (options & Points) {
        parseAll = true;
        return;
    }

    for (auto& geometry: data["geometry"]) {
        std::string objectType = geometry["objectType"];

        if (objectType == "bezierC0" && options & C0Curves) {
            auto& cps = geometry["controlPoints"];
            if (InsertRange(cps.begin(), cps.end(), pointsCnt))
                return;
        }

        else if (objectType == "bezierC2" && options & C2Curves) {
            auto& cps = geometry["deBoorPoints"];
            if (InsertRange(cps.begin(), cps.end(), pointsCnt))
                return;
        }

        else if (objectType == "interpolatedC2" && options & InterpolationCurves) {
            auto& cps = geometry["controlPoints"];
            if (InsertRange(cps.begin(), cps.end(), pointsCnt))
                return;
        }

        else if (objectType == "bezierSurfaceC0" && options & C0Surfaces) {
            for (auto& patch: geometry["patches"]) {
                auto& cps = patch["controlPoints"];
                if (InsertRange(cps.begin(), cps.end(), pointsCnt))
                    return;
            }
        }

        else if (objectType == "bezierSurfaceC2" && options & C2Surfaces) {
            for (auto& patch: geometry["patches"]) {
                auto& cps = patch["controlPoints"];
                if (InsertRange(cps.begin(), cps.end(), pointsCnt))
                    return;
            }
        }
    }
}

bool LoadManager::PointsToParse::ShouldParse(const IdFromFile id) const
{
    return parseAll || idsToParse.contains(id);
}


bool LoadManager::PointsToParse::InsertRange(auto begin, auto end, const int pointsCnt)
{
    idsToParse.insert(begin, end);
    if (idsToParse.size() == pointsCnt) {
        parseAll = true;
        idsToParse.clear();
        return true;
    }

    return false;
}


void LoadManager::LoadPoints(json &data, Coordinator &coordinator, const PointsToParse &pointsToParse)
{
    const auto nameSys = coordinator.GetSystem<NameSystem>();
    const auto pointsSys = coordinator.GetSystem<PointsSystem>();

    for (auto& point: data["points"]) {
        if (!pointsToParse.ShouldParse(point["id"]))
            continue;

        Position pos = ParsePosition(point);

        Entity newPoint = pointsSys->CreatePoint(pos);
        nameSys->SetName(newPoint, point["name"]);

        pointsIdsMap.insert({point["id"], newPoint});
    }
}


void LoadManager::LoadTori(json &data, Coordinator &coordinator)
{
    const auto toriSys = coordinator.GetSystem<ToriSystem>();
    const auto toriRenderingSys = coordinator.GetSystem<ToriRenderingSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    const TorusParameters torusParams {
        .majorRadius = data["largeRadius"],
        .minorRadius = data["smallRadius"],
        .meshDensityMinR = data["samples"]["y"],
        .meshDensityMajR = data["samples"]["x"]
    };

    const Position pos = ParsePosition(data);

    const Entity newTorus = toriSys->AddTorus(pos, torusParams);
    toriRenderingSys->AddEntity(newTorus);

    coordinator.SetComponent(newTorus, ParseRotation(data));
    coordinator.SetComponent(newTorus, ParseScale(data));
    nameSys->SetName(newTorus, data["name"]);
}


void LoadManager::LoadC0Curves(json &data, Coordinator &coordinator)
{
    const auto c0CurveSys = coordinator.GetSystem<C0CurveSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    const auto cps = LoadControlPoints(data["controlPoints"]);
    const Entity newC0Curve = c0CurveSys->CreateC0Curve(cps);
    nameSys->SetName(newC0Curve, data["name"]);
}


void LoadManager::LoadC2Curves(json &data, Coordinator &coordinator)
{
    const auto c2CurveSys = coordinator.GetSystem<C2CurveSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    const auto cps = LoadControlPoints(data["deBoorPoints"]);
    const Entity newC0Curve = c2CurveSys->CreateC2Curve(cps);
    nameSys->SetName(newC0Curve, data["name"]);
}


void LoadManager::LoadInterpolationCurves(json &data, Coordinator &coordinator)
{
    const auto interCurveSys = coordinator.GetSystem<InterpolationCurveSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    const auto cps = LoadControlPoints(data["controlPoints"]);
    const Entity newC0Curve = interCurveSys->CreateCurve(cps);
    nameSys->SetName(newC0Curve, data["name"]);
}


void LoadManager::LoadC0Patches(json &data, Coordinator &coordinator)
{
    const auto c0PatchesSys = coordinator.GetSystem<C0PatchesSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    C0Patches c0Patches(
        data["size"]["y"],
        data["size"]["x"]
    );

    int patchCol = 0;
    int patchRow = 0;

    for (auto& patch: data["patches"]) {
        const int startCol = patchCol * 3;
        const int startRow = patchRow * 3;

        for (int col=0; col < 4; col++) {
            for (int row=0; row < 4; row++) {
                const int idx = row*4 + col;
                int fileID = patch["controlPoints"][idx]["id"];
                const Entity cp = pointsIdsMap[fileID];

                const int globalCol = col + startCol;
                const int globalRow = row + startRow;
                c0Patches.SetPoint(cp, globalRow, globalCol);
            }
        }

        patchCol++;
        if (patchCol >= c0Patches.PatchesInCol()) {
            patchCol = 0;
            patchRow++;
        }
    }

    const Entity newSurface = c0PatchesSys->CreateSurface(c0Patches);
    nameSys->SetName(newSurface, data["name"]);
    coordinator.AddComponent<DrawStd>(newSurface, DrawStd());
}


void LoadManager::LoadC2Patches(json &data, Coordinator &coordinator)
{
    const auto c2PatchesSys = coordinator.GetSystem<C2PatchesSystem>();
    const auto nameSys = coordinator.GetSystem<NameSystem>();

    C2Patches c2Patches(
        data["size"]["y"],
        data["size"]["x"]
    );

    int patchCol = 0;
    int patchRow = 0;

    for (auto& patch: data["patches"]) {

        const int startCol = patchCol;
        const int startRow = patchRow;

        for (int col=0; col < 4; col++) {
            for (int row=0; row < 4; row++) {
                const int idx = row*4 + col;
                const Entity cp = pointsIdsMap[patch["controlPoints"][idx]["id"]];

                const int globalCol = col + startCol;
                const int globalRow = row + startRow;
                c2Patches.SetPoint(cp, globalRow, globalCol);
            }
        }

        patchCol++;
        if (patchCol >= c2Patches.PatchesInCol()) {
            patchCol = 0;
            patchRow++;
        }
    }

    const Entity newSurface = c2PatchesSys->CreateSurface(c2Patches);
    nameSys->SetName(newSurface, data["name"]);
    coordinator.AddComponent<DrawStd>(newSurface, DrawStd());
}


std::vector<Entity> LoadManager::LoadControlPoints(json &data)
{
    std::vector<Entity> cps;
    cps.reserve(data.size());

    for (auto id: data)
        cps.push_back(pointsIdsMap[id["id"]]);

    return cps;
}



