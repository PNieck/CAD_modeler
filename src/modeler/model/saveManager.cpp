#include <CAD_modeler/model/saveManager.hpp>

#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/nameSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/c0CurveSystem.hpp>
#include <CAD_modeler/model/systems/c2CurveSystem.hpp>
#include <CAD_modeler/model/systems/interpolationCurveSystem.hpp>
#include <CAD_modeler/model/systems/c0SurfaceSystem.hpp>
#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>

#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <map>
#include <vector>

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
    float x = json["rotation"]["x"];
    float y = json["rotation"]["y"];
    float z = json["rotation"]["z"];

    return Rotation(x, y, z);
}


Scale ParseScale(const auto& json)
{
    float x = json["scale"]["x"];
    float y = json["scale"]["y"];
    float z = json["scale"]["z"];

    return Scale(
        x, y, z
    );
}


void SaveManager::LoadScene(const std::string &path, Coordinator &coordinator)
{
    using LoadedId = int;

    std::ifstream f(path);
    json data = json::parse(f);

    auto pointsSys = coordinator.GetSystem<PointsSystem>();
    auto toriSys = coordinator.GetSystem<ToriSystem>();
    auto c0CurveSys = coordinator.GetSystem<C0CurveSystem>();
    auto c2CurveSys = coordinator.GetSystem<C2CurveSystem>();
    auto interCurveSys = coordinator.GetSystem<InterpolationCurveSystem>();
    auto c0SurfaceSys = coordinator.GetSystem<C0SurfaceSystem>();
    auto c2SurfaceSys = coordinator.GetSystem<C2SurfaceSystem>();
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
                geometry["size"]["x"],
                geometry["size"]["y"]
            );

            int patchCol = 0;
            int patchRow = 0;

            for (auto& patch: geometry["patches"]) {
                
                int startCol = patchCol * 3;
                int startRow = patchRow * 3;

                for (int col=0; col < 4; col++) {
                    for (int row=0; row < 4; row++) {
                        int idx = row*4 + col;
                        Entity cp = idMap[patch["controlPoints"][idx]["id"]];

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

            Entity newSurface = c0SurfaceSys->CreateSurface(c0Patches);
            nameSys->SetName(newSurface, geometry["name"]);
        }

        else if (objectType == "bezierSurfaceC2") {
            C2Patches c2Patches(
                geometry["size"]["x"],
                geometry["size"]["y"]
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

            Entity newSurface = c2SurfaceSys->CreateSurface(c2Patches);
            nameSys->SetName(newSurface, geometry["name"]);
        }
    }
}


void SaveManager::SaveScene(const std::string &path, Coordinator &coordinator)
{
}
