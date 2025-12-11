#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/linearInterpolator.hpp"
#include "CAD_modeler/utilities/lineSegment2D.hpp"


void MillingPathsDesigner::GeneratePathsForRightEye(MillingMachinePathsBuilder &builder, const MillingCutter &cutter)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity rightEye = nameSystem->EntityFromName("right eye");

    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);
    const Entity rightEyeOffset = equidistanceC2System->AddSurface(rightEye, -cutter.radius);

    auto boundaryCurve = GetBoundaryCurveForRightEye(cutter, torsoOffset, rightEyeOffset);

    std::vector<alg::Vec2> insidePoints;

    const float maxU = c2PatchesSystem->MaxU(rightEye);
    const float maxV = c2PatchesSystem->MaxV(rightEye);

    float boundaryMaxU = -1.f;
    float boundaryMinU = std::numeric_limits<float>::infinity();
    for (const auto& p: boundaryCurve) {
        if (p.X() > boundaryMaxU)
            boundaryMaxU = p.X();

        if (p.X() < boundaryMinU)
            boundaryMinU = p.X();
    }

    constexpr float stepU = 0.05f;
    constexpr float stepV = 0.05f;

    constexpr int maxPointsV = 121;
    static_assert(maxPointsV % 2 == 1);

    const float uLen = maxU - boundaryMaxU;
    int stepsUCnt = static_cast<int>(std::floor(uLen / stepU));
    const float stepULen = uLen / stepsUCnt;

    const LinearInterpolator pointsCntCalc(0, (maxPointsV-1)/2, uLen);

    for (int actStepU = 1; actStepU <= stepsUCnt; actStepU++) {
        float u = maxU - static_cast<float>(actStepU) * stepULen;

        insidePoints.emplace_back(u, maxV/2.f);

        int pointsCnt = std::round(pointsCntCalc.Interpolate(maxU - u));
        float stepVLen = maxV / 2.f / (pointsCnt+1);

        for (int i = 1; i <= pointsCnt; i++) {
            float v = maxV/2.f - static_cast<float>(i) * stepVLen;

            insidePoints.emplace_back(u, v);
        }

        for (int i = 1; i <= pointsCnt; i++) {
            float v = maxV - static_cast<float>(i) * stepVLen;

            insidePoints.emplace_back(u, v);
        }

        insidePoints.emplace_back(u, maxV/2.f);
    }

    // InterCurveToFile("InsidePoints.csv", insidePoints);

    // First position
    auto firstPos = GlobalPosition(rightEyeOffset, insidePoints.front(), cutter);;
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // Rest of position
    for (const auto& p: insidePoints)
        builder.AddPosition(GlobalPosition(rightEyeOffset, p, cutter));

    std::vector<alg::Vec2> restInsidePoints;

    float startU = boundaryMaxU - stepULen;
    float endU = boundaryMinU;
    stepsUCnt = std::ceil((startU - endU) / stepULen);

    float stepVLen = (maxV - stepV) / maxPointsV;

    for (int actStepU = 0; actStepU < stepsUCnt; actStepU++) {
        float u = startU - static_cast<float>(actStepU) * stepULen;

        for (int actStepV = 0; actStepV < maxPointsV; actStepV++) {
            float v = stepV/2.f + static_cast<float>(actStepV) * stepVLen;

            LineSegment2D seg(u, v, u, -1.f);

            int intersectionsCnt = 0;
            for (size_t i = 1; i < boundaryCurve.size(); i++) {
                auto const& actPoint = boundaryCurve[i];
                auto const& prevPoint = boundaryCurve[i-1];

                LineSegment2D boundarySeg(actPoint, prevPoint);
                if (boundarySeg.Length() > 1.f)
                    continue;

                if (LineSegment2D::AreIntersecting(boundarySeg, seg))
                    intersectionsCnt++;
            }

            auto const& actPoint = boundaryCurve.back();
            auto const& prevPoint = boundaryCurve.front();

            LineSegment2D boundarySeg(actPoint, prevPoint);
            if (boundarySeg.Length() < 1.f && LineSegment2D::AreIntersecting(boundarySeg, seg))
                intersectionsCnt++;

            float u348 = boundaryCurve[348].X();
            LineSegment2D additionalSeg(u348, 0.f, boundaryMaxU, 0.f);
            if (LineSegment2D::AreIntersecting(additionalSeg, seg))
                intersectionsCnt++;

            if (intersectionsCnt % 2 == 1)
                restInsidePoints.emplace_back(u, v);
        }
    }

    // InterCurveToFile("RestOfPoints.csv", restInsidePoints);

    AddPointsToBuilder(insidePoints, builder, cutter, rightEyeOffset, 2481, 2518);
    AddPointsToBuilder(restInsidePoints, builder, cutter, rightEyeOffset, 22, 0);
    AddPointsToBuilder(restInsidePoints, builder, cutter, rightEyeOffset, 39, 23);
    AddPointsToBuilder(restInsidePoints, builder, cutter, rightEyeOffset, 23, 33);
    AddPointsToBuilder(restInsidePoints, builder, cutter, rightEyeOffset, 56, 62);
    AddPointsToBuilder(restInsidePoints, builder, cutter, rightEyeOffset, 40, 55);

    const auto& lastInternalPoint = restInsidePoints[55];

    auto boundaryPoints = BoundaryPointsFromInternalPoint(lastInternalPoint, boundaryCurve);
    boundaryPoints = PostProcessBoundary(boundaryPoints, 0.01f);
    for (auto const& point : boundaryPoints)
        builder.AddPosition(GlobalPosition(rightEyeOffset, point, cutter));

    // Last position
    auto lastPos = builder.GetLastPosition();
    lastPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(lastPos);

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(rightEyeOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForRightEye(const MillingCutter &cutter, Entity torsoOffset, Entity rightEyeOffset)
{
    const Entity rightEyeTorsoInter = intersectionSystem->FindIntersection(rightEyeOffset, torsoOffset, 1e-3).value();

    auto const& rightEyeTorsoCurve = coordinator.GetComponent<IntersectionCurve>(rightEyeTorsoInter);
    const std::vector<alg::Vec2> rightEyeTorsoCurveNorm = GetPointsVec(rightEyeTorsoCurve);

    coordinator.DestroyEntity(rightEyeTorsoInter);

    InterCurveToFile("RightEyeTorsoBoundary.csv", rightEyeTorsoCurveNorm);

    return rightEyeTorsoCurveNorm;
}
