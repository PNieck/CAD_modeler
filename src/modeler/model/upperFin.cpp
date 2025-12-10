#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/model/components/wraps.hpp"

#include "CAD_modeler/utilities/lineSegment2D.hpp"
#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/circularVecWrap.hpp"


void MillingPathsDesigner::GeneratePathsForUpperFin(MillingMachinePathsBuilder &builder, const MillingCutter &cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity upperFin = nameSystem->EntityFromName("upper fin");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity upperFinOffset = equidistanceC0System->AddSurface(upperFin, -cutter.radius);

    auto boundaryCurve = GetBoundaryCurveForUpperFin(cutter, torsoOffset, upperFinOffset, upperFin);

    std::vector<alg::Vec2> insidePoints;

    const float maxU = equidistanceC0System->MaxU(upperFinOffset);
    const float maxV = equidistanceC0System->MaxV(upperFinOffset);

    constexpr float stepU = 0.005f;
    constexpr float stepV = 0.005f;

    constexpr float startV = stepV / 2.f;

    const int stepsVCnt = static_cast<int>(std::floor((maxV - startV) / stepV)) + 1;

    for (int actStepV = 0; actStepV < stepsVCnt; actStepV++) {
        float v = startV + static_cast<float>(actStepV) * stepV;

        LineSegment2D vSeg(0.f, v, maxU, v);

        alg::Vec2 intersectionPoint;
        bool intersectionFound = false;
        for (size_t i = 1; i < boundaryCurve.size(); i++) {
            auto const& actPoint = boundaryCurve[i];
            auto const& prevPoint = boundaryCurve[i-1];

            LineSegment2D seg(actPoint, prevPoint);
            if (seg.Length() > 1.f)
                continue;

            if (LineSegment2D::AreIntersecting(vSeg, seg, intersectionPoint)) {
                intersectionFound = true;
                break;
            }
        }

        if (!intersectionFound) {
            auto const& firstPoint = boundaryCurve.front();
            auto const& lastPoint = boundaryCurve.back();

            LineSegment2D seg(firstPoint, lastPoint);
            if (seg.Length() < 1.f && LineSegment2D::AreIntersecting(vSeg, seg, intersectionPoint))
                intersectionFound = true;
        }

        if (!intersectionFound)
            throw std::runtime_error("No intersection found for left eye");

        float endU = intersectionPoint.X();
        float startU = maxU - stepU / 2.f;

        std::vector<alg::Vec2> vStrip;

        for (int actStepU = 0; ; actStepU++) {
            float u = startU - static_cast<float>(actStepU) * stepU;
            if (u < endU)
                break;

            vStrip.emplace_back(u, v);
        }

        if (actStepV % 2 == 1)
            std::ranges::reverse(vStrip);

        insidePoints.insert(insidePoints.end(), vStrip.begin(), vStrip.end());

        if (actStepV == stepsVCnt-1) {
            std::ranges::reverse(vStrip);
            insidePoints.insert(insidePoints.end(), vStrip.begin(), vStrip.end());
        }
    }

    // InterCurveToFile("InsidePoints.csv", insidePoints);

    // First position
    auto firstPos = GlobalPosition(upperFinOffset, insidePoints.front(), cutter);
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // Inside Positions
    for (const auto& p: insidePoints)
        builder.AddPosition(GlobalPosition(upperFinOffset, p, cutter));

    // Boundary positions
    const std::unordered_map<std::string, Entity> offsetSurfaces = {
        {"TorsoOffset", torsoOffset},
        {"UpperFinOffset", upperFinOffset},
        {"UpperFin", upperFin},
    };

    auto properBoundary = GetBoundaryCurveForTorsoUpperFinIntersection(cutter, offsetSurfaces);
    properBoundary = PostProcessBoundary(properBoundary, 0.01f);
    CircularVecWrap circularBoundary(properBoundary);

    // InterCurveToFile("ProperBoundary.csv", properBoundary);

    auto const& lastInsidePos = builder.GetLastPosition();
    int minDistBoundIdx = 0;
    float minDist = std::numeric_limits<float>::infinity();

    for (int i=0; i < properBoundary.size(); ++i) {
        auto boundaryPos = GlobalPosition(torsoOffset, properBoundary[i], cutter);
        float dist = alg::Distance(boundaryPos.vec, lastInsidePos.vec);
        if (dist < minDist) {
            minDistBoundIdx = i;
            minDist = dist;
        }
    }

    for (int i = 0; i < properBoundary.size(); ++i) {
        const int idx = minDistBoundIdx + i;
        builder.AddPosition(GlobalPosition(torsoOffset, circularBoundary[idx], cutter));
    }

    const int idx = minDistBoundIdx + properBoundary.size();
    builder.AddPosition(GlobalPosition(torsoOffset, circularBoundary[idx], cutter));

    // Last position
    auto lastPos = builder.GetLastPosition();
    lastPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(lastPos);

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(upperFinOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForUpperFin(const MillingCutter &cutter, Entity torsoOffset, Entity upperFinOffset, Entity upperFin)
{
    coordinator.DeleteComponent<WrapV>(upperFinOffset);
    coordinator.DeleteComponent<WrapV>(upperFin);

    const Entity upperFinTorsoInter = intersectionSystem->FindIntersection(upperFinOffset, torsoOffset, 1e-3).value();

    coordinator.AddComponent<WrapV>(upperFinOffset, WrapV());
    coordinator.AddComponent<WrapV>(upperFin, WrapV());

    auto const& upperFinTorsoCurve = coordinator.GetComponent<IntersectionCurve>(upperFinTorsoInter);
    const std::vector<alg::Vec2> upperFinTorsoCurveNorm = GetPointsVec(upperFinTorsoCurve);

    InterCurveToFile("UpperFinTorsoBoundary.csv", upperFinTorsoCurveNorm);

    coordinator.DestroyEntity(upperFinTorsoInter);

    return upperFinTorsoCurveNorm;
}
