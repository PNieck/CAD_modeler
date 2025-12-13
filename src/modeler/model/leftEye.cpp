#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/model/millingPathsDesigner/curveWithAABB.hpp"

#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/lineSegment2D.hpp"
#include "CAD_modeler/utilities/linearInterpolator.hpp"

#include <iostream>


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
    auto firstPos = GlobalPosition(leftEyeOffset, insidePoints.front(), cutter);;
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // Rest of position
    for (const auto& p: insidePoints)
        builder.AddPosition(GlobalPosition(leftEyeOffset, p, cutter));

    insidePoints.clear();

    float startU = boundaryMaxU - stepULen;
    float endU = boundaryMinU;
    stepsUCnt = std::floor((startU - endU) / stepULen);

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

            float u385 = boundaryCurve[385].X();
            LineSegment2D additionalSeg(u385, 0.f, boundaryMaxU, 0.f);
            if (LineSegment2D::AreIntersecting(additionalSeg, seg))
                intersectionsCnt++;

            if (intersectionsCnt % 2 == 1)
                insidePoints.emplace_back(u, v);
        }
    }

    // InterCurveToFile("RestOfPoints.csv", insidePoints);

    AddPointsToBuilder(insidePoints, builder, cutter, leftEyeOffset, 61, 82);
    AddPointsToBuilder(insidePoints, builder, cutter, leftEyeOffset, 82, 0);
    AddPointsToBuilder(insidePoints, builder, cutter, leftEyeOffset, 0, 10);
    AddPointsToBuilder(insidePoints, builder, cutter, leftEyeOffset, 83, 120);

    const auto& lastInternalPoint = insidePoints[120];

    const auto boundaryPoints = BoundaryPointsFromInternalPoint(lastInternalPoint, boundaryCurve);

    for (auto const& point : boundaryPoints)
        builder.AddPosition(GlobalPosition(leftEyeOffset, point, cutter));

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

    // InterCurveToFile("LeftEyeTorsoBoundary.csv", leftEyeTorsoCurveNorm);

    coordinator.DestroyEntity(leftEyeTorsoInter);

    return leftEyeTorsoCurveNorm;
}
