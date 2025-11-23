#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/utilities/lineSegment2D.hpp"
#include "CAD_modeler/utilities/toFile.hpp"

#include "CAD_modeler/model/millingPathsDesigner/insideFiller.hpp"

// TODO: remove
#include <iostream>


void MillingPathsDesigner::GeneratePathsForRightFin(MillingMachinePathsBuilder& builder, const MillingCutter& cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity rightFin = nameSystem->EntityFromName("right fin");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity leftFinOffset = equidistanceC2System->AddSurface(rightFin, -cutter.radius);

    const auto boundaryCurve = GetBoundaryCurveForRightFin(cutter, torsoOffset, leftFinOffset);

    const InsideFiller filler(0.f, 3.f, 5.5f, 1.5f, 0.05f, 0.05f, c2PatchesSystem->MaxU(rightFin), c2PatchesSystem->MaxV(rightFin));
    const auto points = filler.Fill(boundaryCurve);

    InterCurveToFile("InsidePoints.csv", points);

    const auto combined = ConnectInsidePointToBoundary(points, boundaryCurve);

    builder.AddPosition(millingSettings.initCutterPos);

    for (const auto& p: combined) {
        const float u = p.X();
        const float v = p.Y();

        builder.AddPosition(equidistanceC2System->PointOnSurface(leftFinOffset, u, v).vec - alg::Vec3::UnitY() * cutter.radius);
    }

    builder.AddPosition(millingSettings.initCutterPos);

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(leftFinOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForRightFin(
    const MillingCutter &cutter, const Entity torsoOffset, const Entity rightFinOffset
) {
    const Entity baseOffset = c0PatchesSystem->CreatePlane(
        alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness + cutter.radius, -materialParameters.zLen/2.f),
        alg::Vec3::UnitY(),
        materialParameters.xLen, materialParameters.zLen
    );

    const Entity finBaseInter = intersectionSystem->FindIntersection(rightFinOffset, baseOffset, 1e-3).value();
    const Entity finTorsoInter = intersectionSystem->FindIntersection(rightFinOffset, torsoOffset, 1e-3).value();

    auto const& finBaseCurve = coordinator.GetComponent<IntersectionCurve>(finBaseInter);
    auto const& finTorsoCurve = coordinator.GetComponent<IntersectionCurve>(finTorsoInter);

    const std::vector<alg::Vec2> finBaseCurveNorm = GetPointsVec(finBaseCurve);
    const std::vector<alg::Vec2> finTorsoCurveNorm = GetPointsVec(finTorsoCurve);
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
            if (torsoSeg.Length() > 1.0f) {
                boundary.emplace_back(prevBasePoint);
                continue;
            }

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

    // InterCurveToFile("finTorsoIntersections.csv", finTorsoCurve);
    // InterCurveToFile("finBaseIntersections.csv", finBaseCurve);
    // InterCurveToFile("boundary.csv", boundary);
    //
    // InterCurveToFile("finTorsoIntersections_normalized.csv", finTorsoCurveNorm);
    // InterCurveToFile("finBaseIntersections_normalized.csv", finBaseCurveNorm);

    // std::cout << "Max U: " << equidistanceC2System->MaxU(rightFinOffset) << " Max V: " << equidistanceC2System->MaxV(rightFinOffset) << std::endl;

    Update();
    coordinator.DestroyEntity(finBaseInter);
    coordinator.DestroyEntity(finTorsoInter);
    coordinator.DestroyEntity(baseOffset);

    return boundary;
}
