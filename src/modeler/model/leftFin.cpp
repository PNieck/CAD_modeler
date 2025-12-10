#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/millingPathsDesigner/insideFiller.hpp>

#include <CAD_modeler/utilities/lineSegment2D.hpp>


// TODO: delete
#include <iostream>
#include <CAD_modeler/utilities/toFile.hpp>


void MillingPathsDesigner::GeneratePathsForLeftFin(MillingMachinePathsBuilder &builder, const MillingCutter& cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity leftFin = nameSystem->EntityFromName("left fin");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity leftFinOffset = equidistanceC2System->AddSurface(leftFin, -cutter.radius);

    auto boundaryCurve = GetBoundaryCurveForLeftFin(cutter, torsoOffset, leftFinOffset);

    const InsideFiller filler(0.f, 3.f, 2.f, 5.f, 0.05f, 0.05f, c2PatchesSystem->MaxU(leftFin), c2PatchesSystem->MaxV(leftFin));
    const auto points = filler.Fill(boundaryCurve);

    // InterCurveToFile("InsidePoints.csv", points);

    boundaryCurve = PostProcessBoundary(boundaryCurve, 0.01f);
    const auto combined = ConnectInsidePointToBoundary(points, boundaryCurve);

    // First position
    auto firstPos = GlobalPosition(leftFinOffset, combined.front(), cutter);
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // Rest of positions
    for (const auto& p: combined)
        builder.AddPosition(GlobalPosition(leftFinOffset, p, cutter));

    // Last position
    auto lastPos = builder.GetLastPosition();
    lastPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(lastPos);

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(leftFinOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForLeftFin(const MillingCutter &cutter, Entity torsoOffset, Entity leftFinOffset)
{
    const Entity baseOffset = c0PatchesSystem->CreatePlane(
        alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness + cutter.radius, -materialParameters.zLen/2.f),
        alg::Vec3::UnitY(),
        materialParameters.xLen, materialParameters.zLen
    );

    const Entity leftFinBaseInter = intersectionSystem->FindIntersection(leftFinOffset, baseOffset, 1e-3).value();
    const Entity leftFinTorsoInter = intersectionSystem->FindIntersection(leftFinOffset, torsoOffset, 1e-3).value();

    auto const& leftFinBaseCurve = coordinator.GetComponent<IntersectionCurve>(leftFinBaseInter);
    auto const& leftFinTorsoCurve = coordinator.GetComponent<IntersectionCurve>(leftFinTorsoInter);

    const std::vector<alg::Vec2> finBaseCurveNorm = GetPointsVec(leftFinBaseCurve);
    const std::vector<alg::Vec2> finTorsoCurveNorm = GetPointsVec(leftFinTorsoCurve);
    std::vector<alg::Vec2> boundary;

    size_t actBaseIdx = 1;
    size_t actTorsoIdx = 1;
    bool intersectionFound = false;
    for (; actBaseIdx < finBaseCurveNorm.size() && !intersectionFound; ++actBaseIdx) {
        const auto& actBasePoint = finBaseCurveNorm[actBaseIdx];
        const auto& prevBasePoint = finBaseCurveNorm[actBaseIdx-1];

        LineSegment2D baseSeg(actBasePoint, prevBasePoint);

        for (actTorsoIdx = 1; actTorsoIdx < finTorsoCurveNorm.size(); ++actTorsoIdx) {
            const auto& actTorsoPoint = finTorsoCurveNorm[actTorsoIdx];
            const auto& prevTorsoPoint = finTorsoCurveNorm[actTorsoIdx-1];

            LineSegment2D torsoSeg(actTorsoPoint, prevTorsoPoint);
            if (torsoSeg.Length() > 1.0f)
                continue;

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(baseSeg, torsoSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                break;
            }
        }
    }

    intersectionFound = false;
    for (; actBaseIdx < finBaseCurveNorm.size() && !intersectionFound; ++actBaseIdx) {
        const auto& actBasePoint = finBaseCurveNorm[actBaseIdx];
        const auto& prevBasePoint = finBaseCurveNorm[actBaseIdx-1];

        LineSegment2D baseSeg(actBasePoint, prevBasePoint);

        for (actTorsoIdx = 1; actTorsoIdx < finTorsoCurveNorm.size(); ++actTorsoIdx) {
            const auto& actTorsoPoint = finTorsoCurveNorm[actTorsoIdx];
            const auto& prevTorsoPoint = finTorsoCurveNorm[actTorsoIdx-1];

            LineSegment2D torsoSeg(actTorsoPoint, prevTorsoPoint);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(baseSeg, torsoSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                break;
            }
        }

        if (!intersectionFound)
            boundary.emplace_back(prevBasePoint);
    }

    actTorsoIdx++;
    intersectionFound = false;
    for (; actTorsoIdx < finTorsoCurveNorm.size() && !intersectionFound; ++actTorsoIdx) {
        const auto& actTorsoPoint = finTorsoCurveNorm[actTorsoIdx];
        const auto& prevTorsoPoint = finTorsoCurveNorm[actTorsoIdx-1];

        LineSegment2D torsoSeg(actTorsoPoint, prevTorsoPoint);
        if (torsoSeg.Length() > 1.0f) {
            boundary.emplace_back(actTorsoPoint);
            continue;
        }

        for (actBaseIdx = 1; actBaseIdx < finBaseCurveNorm.size(); ++actBaseIdx) {
            const auto& actBasePoint = finBaseCurveNorm[actBaseIdx];
            const auto& prevBasePoint = finBaseCurveNorm[actBaseIdx-1];

            LineSegment2D baseSeg(actBasePoint, prevBasePoint);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(baseSeg, torsoSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                break;
            }
        }

        if (!intersectionFound)
            boundary.emplace_back(actTorsoPoint);
    }

    // InterCurveToFile("finTorsoIntersections.csv", leftFinTorsoCurve);
    // InterCurveToFile("finBaseIntersections.csv", leftFinBaseCurve);
    // InterCurveToFile("boundary.csv", boundary);
    //
    // InterCurveToFile("finTorsoIntersections_normalized.csv", finTorsoCurveNorm);
    // InterCurveToFile("finBaseIntersections_normalized.csv", finBaseCurveNorm);
    //
    // std::cout << "Max U: " << equidistanceC2System->MaxU(leftFinOffset) << " Max V: " << equidistanceC2System->MaxV(leftFinOffset) << std::endl;

    Update();

    coordinator.DestroyEntity(leftFinBaseInter);
    coordinator.DestroyEntity(leftFinTorsoInter);
    coordinator.DestroyEntity(baseOffset);

    return boundary;
}

