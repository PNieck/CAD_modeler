#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/components/wraps.hpp>

#include "CAD_modeler/utilities/toFile.hpp"


void MillingPathsDesigner::GeneratePathsForTorso(MillingMachinePathsBuilder &builder, const MillingCutter &cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity rightFin = nameSystem->EntityFromName("right fin");
    const Entity leftFin = nameSystem->EntityFromName("left fin");
    const Entity upperFin = nameSystem->EntityFromName("upper fin");
    const Entity rightEye = nameSystem->EntityFromName("right eye");
    const Entity leftEye = nameSystem->EntityFromName("left eye");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity upperFinOffset = equidistanceC0System->AddSurface(upperFin, -cutter.radius);

    const std::unordered_map<std::string, Entity> offsetSurfaces = {
        {"TorsoOffset", torsoOffset},
        {"UpperFinOffset", upperFinOffset},
        { "UpperFin", upperFin}
    };

    GetBoundaryCurveForTorso(cutter, offsetSurfaces);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForTorso(const MillingCutter& cutter, std::unordered_map<std::string, Entity> offsetSurfaces)
{
    const Entity baseOffset = c0PatchesSystem->CreatePlane(
        alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness + cutter.radius, -materialParameters.zLen/2.f),
        alg::Vec3::UnitY(),
        materialParameters.xLen, materialParameters.zLen
    );

    const Entity upperFin = offsetSurfaces["UpperFin"];
    const Entity upperFinOffset = offsetSurfaces["UpperFinOffset"];
    const Entity torsoOffset = offsetSurfaces["TorsoOffset"];

    //coordinator.DeleteComponent<WrapU>(upperFinOffset);
    coordinator.DeleteComponent<WrapV>(upperFinOffset);
    coordinator.DeleteComponent<WrapV>(upperFin);

    const Entity torsoUpperFinInter = intersectionSystem->FindIntersection(torsoOffset, upperFinOffset, 1e-3).value();
    auto const& torsoUpperFinCurve = coordinator.GetComponent<IntersectionCurve>(torsoUpperFinInter);

    InterCurveToFile("torsoUpperFinIntersections.csv", torsoUpperFinCurve);

    Update();
    coordinator.DestroyEntity(baseOffset);

    return {};
}
