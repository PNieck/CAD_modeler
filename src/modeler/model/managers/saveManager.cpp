#include <CAD_modeler/model/managers/saveManager.hpp>

#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/nameSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/c0CurveSystem.hpp>
#include <CAD_modeler/model/systems/c2CurveSystem.hpp>
#include <CAD_modeler/model/systems/interpolationCurveSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/wraps.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <map>
#include <stack>

#include "CAD_modeler/model/systems/toriRenderingSystem.hpp"


using json = nlohmann::json;


Position ParsePosition(const auto& json)
{
    return Position(
        json["position"]["x"],
        json["position"]["y"],
        json["position"]["z"]
    );
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

    return {
        x, y, z
    };
}


void SaveManager::LoadScene(const std::string &path, Coordinator &coordinator)
{
    using LoadedId = int;

    std::ifstream f(path);
    json data = json::parse(f);

    auto pointsSys = coordinator.GetSystem<PointsSystem>();
    auto toriSys = coordinator.GetSystem<ToriSystem>();
    auto toriRenderingSys = coordinator.GetSystem<ToriRenderingSystem>();
    auto c0CurveSys = coordinator.GetSystem<C0CurveSystem>();
    auto c2CurveSys = coordinator.GetSystem<C2CurveSystem>();
    auto interCurveSys = coordinator.GetSystem<InterpolationCurveSystem>();
    auto c0PatchesSys = coordinator.GetSystem<C0PatchesSystem>();
    auto c2PatchesSys = coordinator.GetSystem<C2PatchesSystem>();
    auto nameSys = coordinator.GetSystem<NameSystem>();

    std::map<LoadedId, Entity> idMap;

    // Loading points
    for (auto& point: data["points"]) {
        Position pos = ParsePosition(point);

        Entity newPoint = pointsSys->CreatePoint(pos);
        nameSys->SetName(newPoint, point["name"]);

        idMap.insert({point["id"], newPoint});
    }

    // Loading geometry
    for (auto& geometry: data["geometry"]) {
        std::string objectType = geometry["objectType"];

        if (objectType == "torus") {
            TorusParameters torusParams {
                .majorRadius = geometry["largeRadius"],
                .minorRadius = geometry["smallRadius"],
                .meshDensityMinR = geometry["samples"]["y"],
                .meshDensityMajR = geometry["samples"]["x"]
            };

            Position pos = ParsePosition(geometry);

            Entity newTorus = toriSys->AddTorus(pos, torusParams);
            toriRenderingSys->AddEntity(newTorus);

            coordinator.SetComponent(newTorus, ParseRotation(geometry));
            coordinator.SetComponent(newTorus, ParseScale(geometry));
            nameSys->SetName(newTorus, geometry["name"]);
        }

        else if (objectType == "bezierC0") {
            std::vector<Entity> cps;

            for (auto id: geometry["controlPoints"])
                cps.push_back(idMap[id["id"]]);

            Entity newC0Curve = c0CurveSys->CreateC0Curve(cps);
            nameSys->SetName(newC0Curve, geometry["name"]);
        }

        else if (objectType == "bezierC2") {
            std::vector<Entity> cps;

            for (auto id: geometry["deBoorPoints"])
                cps.push_back(idMap[id["id"]]);

            Entity newC2Curve = c2CurveSys->CreateC2Curve(cps);
            nameSys->SetName(newC2Curve, geometry["name"]);
        }

        else if (objectType == "interpolatedC2") {
            std::vector<Entity> cps;

            for (auto id: geometry["controlPoints"]) {
                int idVal = id["id"];
                cps.push_back(idMap[idVal]);
            }

            Entity newInterCurve = interCurveSys->CreateCurve(cps);
            nameSys->SetName(newInterCurve, geometry["name"]);
        }


        else if (objectType == "bezierSurfaceC0") {
            C0Patches c0Patches(
                geometry["size"]["y"],
                geometry["size"]["x"]
            );

            int patchCol = 0;
            int patchRow = 0;

            for (auto& patch: geometry["patches"]) {

                int startCol = patchCol * 3;
                int startRow = patchRow * 3;

                for (int col=0; col < 4; col++) {
                    for (int row=0; row < 4; row++) {
                        int idx = row*4 + col;
                        int fileID = patch["controlPoints"][idx]["id"];
                        Entity cp = idMap[fileID];

                        int globalCol = col + startCol;
                        int globalRow = row + startRow;
                        c0Patches.SetPoint(cp, globalRow, globalCol);
                    }
                }

                patchCol++;
                if (patchCol >= c0Patches.PatchesInCol()) {
                    patchCol = 0;
                    patchRow++;
                }
            }

            Entity newSurface = c0PatchesSys->CreateSurface(c0Patches);
            nameSys->SetName(newSurface, geometry["name"]);
            coordinator.AddComponent<DrawStd>(newSurface, DrawStd());
        }

        else if (objectType == "bezierSurfaceC2") {
            C2Patches c2Patches(
                geometry["size"]["y"],
                geometry["size"]["x"]
            );

            int patchCol = 0;
            int patchRow = 0;

            for (auto& patch: geometry["patches"]) {

                int startCol = patchCol;
                int startRow = patchRow;

                for (int col=0; col < 4; col++) {
                    for (int row=0; row < 4; row++) {
                        int idx = row*4 + col;
                        Entity cp = idMap[patch["controlPoints"][idx]["id"]];

                        int globalCol = col + startCol;
                        int globalRow = row + startRow;
                        c2Patches.SetPoint(cp, globalRow, globalCol);
                    }
                }

                patchCol++;
                if (patchCol >= c2Patches.PatchesInCol()) {
                    patchCol = 0;
                    patchRow++;
                }
            }

            Entity newSurface = c2PatchesSys->CreateSurface(c2Patches);
            nameSys->SetName(newSurface, geometry["name"]);
        }
    }
}


void SavePosition(auto& json, const Position& pos)
{
    json["position"]["x"] = pos.GetX();
    json["position"]["y"] = pos.GetY();
    json["position"]["z"] = pos.GetZ();
}


void SaveRotation(auto& json, const Rotation& rot)
{
    auto ypr = rot.GetRollPitchRoll();

    json["rotation"]["x"] = ypr.X();
    json["rotation"]["y"] = ypr.Y();
    json["rotation"]["z"] = ypr.Z();
}


void SaveScale(auto& json, const Scale& scale)
{
    json["scale"]["x"] = scale.GetX();
    json["scale"]["y"] = scale.GetY();
    json["scale"]["z"] = scale.GetZ();
}


void SaveManager::SaveScene(const std::string &path, Coordinator &coordinator)
{
    auto pointsSys = coordinator.GetSystem<PointsSystem>();
    auto toriSys = coordinator.GetSystem<ToriSystem>();
    auto c0CurveSys = coordinator.GetSystem<C0CurveSystem>();
    auto c2CurveSys = coordinator.GetSystem<C2CurveSystem>();
    auto interCurveSys = coordinator.GetSystem<InterpolationCurveSystem>();
    auto c0PatchesSys = coordinator.GetSystem<C0PatchesSystem>();
    auto c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    auto nameSys = coordinator.GetSystem<NameSystem>();

    std::stack<Entity> fakeIDs;
    std::ofstream output(path);
    json mainJson;

    for (Entity point: pointsSys->GetEntities()) {
        json pointJson;

        pointJson["id"] = point;
        pointJson["name"] = nameSys->GetName(point);
        SavePosition(pointJson, coordinator.GetComponent<Position>(point));

        mainJson["points"].push_back(pointJson);
    }

    for (Entity torus: toriSys->GetEntities()) {
        json torusJson;

        torusJson["objectType"] = "torus";
        torusJson["id"] = torus;
        torusJson["name"] = nameSys->GetName(torus);

        const auto& params = coordinator.GetComponent<TorusParameters>(torus);
        torusJson["smallRadius"] = params.minorRadius;
        torusJson["largeRadius"] = params.majorRadius;
        torusJson["samples"]["x"] = params.meshDensityMajR;
        torusJson["samples"]["y"] = params.meshDensityMinR;

        SavePosition(torusJson, coordinator.GetComponent<Position>(torus));
        SaveRotation(torusJson, coordinator.GetComponent<Rotation>(torus));
        SaveScale(torusJson, coordinator.GetComponent<Scale>(torus));

        mainJson["geometry"].push_back(torusJson);
    }

    for (Entity bezierC0: c0CurveSys->GetEntities()) {
        json bezierC0Json;

        bezierC0Json["objectType"] = "bezierC0";
        bezierC0Json["id"] = bezierC0;
        bezierC0Json["name"] = nameSys->GetName(bezierC0);

        const auto& cps = coordinator.GetComponent<CurveControlPoints>(bezierC0);
        for (Entity cp: cps.GetPoints()) {
            json cpJson;
            cpJson["id"] = cp;
            bezierC0Json["controlPoints"].push_back(cpJson);
        }

        mainJson["geometry"].push_back(bezierC0Json);
    }

    for (Entity bezierC2: c2CurveSys->GetEntities()) {
        json bezierC2Json;

        bezierC2Json["objectType"] = "bezierC2";
        bezierC2Json["id"] = bezierC2;
        bezierC2Json["name"] = nameSys->GetName(bezierC2);

        const auto& cps = coordinator.GetComponent<CurveControlPoints>(bezierC2);
        for (Entity cp: cps.GetPoints()) {
            json cpJson;
            cpJson["id"] = cp;
            bezierC2Json["deBoorPoints"].push_back(cpJson);
        }

        mainJson["geometry"].push_back(bezierC2Json);
    }

    for (Entity interCurve: interCurveSys->GetEntities()) {
        json interCurveJson;

        interCurveJson["objectType"] = "interpolatedC2";
        interCurveJson["id"] = interCurve;
        interCurveJson["name"] = nameSys->GetName(interCurve);

        const auto& cps = coordinator.GetComponent<CurveControlPoints>(interCurve);
        for (Entity cp: cps.GetPoints()) {
            json cpJson;
            cpJson["id"] = cp;
            interCurveJson["controlPoints"].push_back(cpJson);
        }

        mainJson["geometry"].push_back(interCurveJson);
    }

    for (Entity surfaceC0: c0PatchesSys->GetEntities()) {
        json surfaceC0Json;

        surfaceC0Json["objectType"] = "bezierSurfaceC0";
        surfaceC0Json["id"] = surfaceC0;
        surfaceC0Json["name"] = nameSys->GetName(surfaceC0);

        const auto& patches = coordinator.GetComponent<C0Patches>(surfaceC0);
        surfaceC0Json["size"]["y"] = patches.PatchesInRow();
        surfaceC0Json["size"]["x"] = patches.PatchesInCol();

        const auto& density = coordinator.GetComponent<PatchesDensity>(surfaceC0);

        for (int rowPatch=0; rowPatch < patches.PatchesInRow(); rowPatch++ ) {
            for (int colPatch=0; colPatch < patches.PatchesInCol(); colPatch++) {
                json patchJson;

                patchJson["objectType"] = "bezierPatchC0";
                Entity fakeID = coordinator.CreateEntity();
                patchJson["id"] = fakeID;
                patchJson["name"] = "";
                fakeIDs.push(fakeID);

                patchJson["samples"]["x"] = density.GetDensity();
                patchJson["samples"]["y"] = density.GetDensity();

                for (int row=0; row < 4; row++) {
                    for (int col=0; col < 4; col++) {
                        json cpJson;
                        Entity cp = patches.GetPoint(
                            rowPatch*3 + row,
                            colPatch*3 + col
                        );

                        cpJson["id"] = cp;
                        patchJson["controlPoints"].push_back(cpJson);

                    }
                }

                surfaceC0Json["patches"].push_back(patchJson);
            }
        }

        surfaceC0Json["parameterWrapped"]["u"] = coordinator.HasComponent<WrapU>(surfaceC0);
        surfaceC0Json["parameterWrapped"]["v"] = coordinator.HasComponent<WrapV>(surfaceC0);

        mainJson["geometry"].push_back(surfaceC0Json);
    }

    for (Entity patchesC2: c2PatchesSystem->GetEntities()) {
        json surfaceC2Json;

        surfaceC2Json["objectType"] = "bezierSurfaceC2";
        surfaceC2Json["id"] = patchesC2;
        surfaceC2Json["name"] = nameSys->GetName(patchesC2);

        const auto& patches = coordinator.GetComponent<C2Patches>(patchesC2);
        surfaceC2Json["size"]["y"] = patches.PatchesInRow();
        surfaceC2Json["size"]["x"] = patches.PatchesInCol();

        const auto& density = coordinator.GetComponent<PatchesDensity>(patchesC2);

        for (int rowPatch=0; rowPatch < patches.PatchesInRow(); rowPatch++ ) {
            for (int colPatch=0; colPatch < patches.PatchesInCol(); colPatch++) {
                json patchJson;

                patchJson["objectType"] = "bezierPatchC2";
                Entity fakeID = coordinator.CreateEntity();
                patchJson["id"] = fakeID;
                patchJson["name"] = "";
                fakeIDs.push(fakeID);

                patchJson["samples"]["x"] = density.GetDensity();
                patchJson["samples"]["y"] = density.GetDensity();

                for (int row=0; row < 4; row++) {
                    for (int col=0; col < 4; col++) {
                        json cpJson;
                        Entity cp = patches.GetPoint(
                            rowPatch + row,
                            colPatch + col
                        );

                        cpJson["id"] = cp;
                        patchJson["controlPoints"].push_back(cpJson);

                    }
                }

                surfaceC2Json["patches"].push_back(patchJson);
            }
        }

        surfaceC2Json["parameterWrapped"]["u"] = coordinator.HasComponent<WrapU>(patchesC2);
        surfaceC2Json["parameterWrapped"]["v"] = coordinator.HasComponent<WrapV>(patchesC2);

        mainJson["geometry"].push_back(surfaceC2Json);
    }

    output << std::setw(4) << mainJson;

    while (!fakeIDs.empty()) {
        coordinator.DestroyEntity(fakeIDs.top());
        fakeIDs.pop();
    }
}
