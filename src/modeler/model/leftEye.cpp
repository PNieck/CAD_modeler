#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/lineSegment2D.hpp"


void MillingPathsDesigner::GeneratePathsForLeftEye(MillingMachinePathsBuilder &builder, const MillingCutter &cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity leftEye = nameSystem->EntityFromName("left eye");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity leftEyeOffset = equidistanceC2System->AddSurface(leftEye, -cutter.radius);

    auto boundaryCurve = GetBoundaryCurveForLeftEye(cutter, torsoOffset, leftEyeOffset);

    std::vector<alg::Vec2> insidePoints;

    const float maxU = c2PatchesSystem->MaxU(leftEye);
    const float maxV = c2PatchesSystem->MaxV(leftEye);

    constexpr float stepU = 0.05f;
    constexpr float stepV = 0.05f;

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

    boundaryCurve = PostProcessBoundary(boundaryCurve, 0.01f);
    const auto combined = ConnectInsidePointToBoundary(insidePoints, boundaryCurve);

    // InterCurveToFile("InsidePoints.csv", insidePoints);

    // First position
    auto firstPos = GlobalPosition(leftEyeOffset, combined.front(), cutter);
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // Rest of position
    for (const auto& p: combined)
        builder.AddPosition(GlobalPosition(leftEyeOffset, p, cutter));

    // Last position
    auto lastPos = builder.GetLastPosition();
    lastPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(lastPos);

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(leftEyeOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForLeftEye(const MillingCutter &cutter, Entity torsoOffset, Entity leftEyeOffset)
{
    const Entity leftEyeTorsoInter = intersectionSystem->FindIntersection(leftEyeOffset, torsoOffset, 1e-3).value();

    auto const& leftEyeTorsoCurve = coordinator.GetComponent<IntersectionCurve>(leftEyeTorsoInter);
    const std::vector<alg::Vec2> leftEyeTorsoCurveNorm = GetPointsVec(leftEyeTorsoCurve);

    InterCurveToFile("LeftEyeTorsoBoundary.csv", leftEyeTorsoCurveNorm);

    coordinator.DestroyEntity(leftEyeTorsoInter);

    return leftEyeTorsoCurveNorm;
}
