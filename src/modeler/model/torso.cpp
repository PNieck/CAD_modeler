#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/components/wraps.hpp>

#include "CAD_modeler/model/millingPathsDesigner/insideFiller.hpp"
#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/circularVecWrap.hpp"
#include "CAD_modeler/utilities/lineSegment2D.hpp"

#include "CAD_modeler/model/millingPathsDesigner/curveWithAABB.hpp"

#include <iostream>


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
    const Entity rightFinOffset = equidistanceC2System->AddSurface(rightFin, -cutter.radius);
    const Entity leftFinOffset = equidistanceC2System->AddSurface(leftFin, -cutter.radius);
    const Entity rightEyeOffset = equidistanceC2System->AddSurface(rightEye, -cutter.radius);
    const Entity leftEyeOffset = equidistanceC2System->AddSurface(leftEye, -cutter.radius);

    const std::unordered_map<std::string, Entity> offsetSurfaces = {
        {"TorsoOffset", torsoOffset},
        {"UpperFinOffset", upperFinOffset},
        {"UpperFin", upperFin},
        {"RightFinOffset", rightFinOffset},
        {"LeftFinOffset", leftFinOffset},
        {"RightEyeOffset", rightEyeOffset},
        {"LeftEyeOffset", leftEyeOffset}
    };

    const auto boundary = GetBoundaryCurveForTorso(cutter, offsetSurfaces);
    const auto holes = GetTorsoHoles(cutter, offsetSurfaces);

    float maxV = c2PatchesSystem->MaxV(torso);

    const auto boundaryAABB = CurveWithAABB(boundary, 2.f, maxV);

    std::vector<CurveWithAABB> holesAABB;
    holesAABB.reserve(holes.size());

    for (auto const& hole : holes)
        holesAABB.emplace_back(hole, 2.f, maxV);

    constexpr float startU = 1.5f;
    constexpr float stopU = 13.5f;

    constexpr float startV = 5.3f;
    constexpr float stopV = 0.7;

    constexpr float stepU = 0.05f;
    constexpr float stepV = 0.05f;

    const int stepsUCnt = static_cast<int>(std::ceil((stopU - startU) / stepU));
    const int stepsVCnt = static_cast<int>(std::ceil((stopV + maxV - startV) / stepV));

    std::vector<alg::Vec2> onSurfacePoints;

    for (int actStepV=0; actStepV < stepsVCnt; actStepV++) {
        for (int actStepU=0; actStepU < stepsUCnt; actStepU++) {
            const float u = startU + static_cast<float>(actStepU) * stepU;
            float v = startV + static_cast<float>(actStepV) * stepV;
            if (v > maxV)
                v -= maxV;

            if (!boundaryAABB.Inside(u, v))
                continue;

            bool insideHole = false;
            for (const auto& hole: holesAABB) {
                if (hole.Inside(u, v)) {
                    insideHole = true;
                    break;
                }
            }

            if (!insideHole)
                onSurfacePoints.emplace_back(u, v);
        }
    }

    InterCurveToFile("InsidePoints.csv", onSurfacePoints);

    std::cout << "indie points len: " << onSurfacePoints.size() << std::endl;

    // First position
    auto firstPos = GlobalPosition(leftFinOffset, onSurfacePoints.front(), cutter);
    firstPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(firstPos);

    // First bay
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 0, 7);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 51, 8);

    // Second bay
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 67, 187);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 66, 52);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 173, 187);

    // Behind left fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 313, 188);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 335, 463);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 623, 491);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 655, 789);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 962, 825);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1000, 1140);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1326, 1182);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1371, 1519);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1719, 1567);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1772, 1996);

    // Ahead of left Fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1771, 1720);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1520, 1566);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1370, 1327);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 1141, 1181);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 999, 963);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 790, 824);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 654, 624);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 464, 490);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 334, 314);

    // Going back up
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 314, 334);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset,
        {490, 654, 824, 999, 1181, 1370, 1566, 1771, 1996});

    // All along the torso
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2221, 1997);

    // Behind left eye
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2222, 2417);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2635, 2443);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2659, 2849);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3061, 2871);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3082, 3271);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3480, 3292);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3501, 3689);

    // Between left eye and upper fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3882, 3830);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4019, 4064);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4240, 4199);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4373, 4411);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4577, 4542);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4707, 4740);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4900, 4870);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5031, 5060);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5221, 5191);

    // To the torso front
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5355, 5419);

    // Ahead of left eye
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5249, 5222);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5061, 5085);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4924, 4901);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4741, 4762);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4598, 4578);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4412, 4431);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4260, 4241);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4065, 4084);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3901, 3883);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3690, 3708);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3500, 3481);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3272, 3291);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3081, 3062);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2850, 2870);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2658, 2636);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 2418, 2442);

    // Going back up
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset,
        {2442, 2658, 2870, 3081, 3291, 3500, 3708, 3901, 4084, 4260, 4431, 4598, 4762, 4924, 5085, 5249, 5419});

    // Between eyes
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5587, 5524);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5692, 5754);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5921, 5859);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6026, 6088);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6257, 6194);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6363, 6428);

    // Ahead of right eye
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6591, 6564);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6729, 6753);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6916, 6894);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7059, 7080);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7249, 7228);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7401, 7421);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7598, 7579);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7764, 7783);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7983, 7963);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8172, 8191);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8400, 8381);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8590, 8609);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8820, 8800);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9012, 9033);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9249, 9227);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9228, 9249);

    // All along the torso
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9474, 9250);

    // Behind the right eye
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9034, 9226);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9011, 8821);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8610, 8799);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8589, 8401);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8192, 8380);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 8171, 7984);

    // Behind the upper fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7784, 7907);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7716, 7599);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7422, 7536);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7361, 7250);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7081, 7191);

    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7025, 6917);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6754, 6861);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6698, 6592);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6429, 6534);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6362, 6258);

    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6089, 6193);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6025, 5922);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5755, 5858);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5691, 5588);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5420, 5523);

    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5354, 5250);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5086, 5190);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5030, 4925);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4763, 4869);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4706, 4599);

    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4432, 4541);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4372, 4261);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4085, 4198);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4018, 3902);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3709, 3829);

    // Around the upper fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3622, 3636);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 3830, 3836);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4019, 4022);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4199, 4202);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4373, 4375);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4542, 4544);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4707, 4710);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 4870, 4872);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5031, 5032);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 5191, 5192);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset,
        {5355, 5524, 5692, 5859, 6026, 6194});
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6364, 6363);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6536, 6535);

    // Between the right eye and upper fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6535, 6563);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6728, 6699);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 6862, 6893);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7058, 7026);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7192, 7227);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7400, 7362);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7537, 7578);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7763, 7717);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 7908, 7962);

    // Go around right eye
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset,
        {8171, 8380, 8589, 8798, 8799, 9011, 9225, 9226, 9442, 9656});

    // Ahead of right fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9655, 9687);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9687, 9633);

    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9839, 9887);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10079, 10035);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10223, 10263);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10441, 10404);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10579, 10613);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10778, 10748);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10910, 10936);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 11083, 11063);

    // Going around right fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset,
        {10910, 10748, 10579, 10405, 10404, 10223});
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10037, 10035);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9841, 9839);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9637, 9633);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9418, 9408);

    // Behind right fin
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9632, 9475);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 9688, 9838);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10034, 9888);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10080, 10222);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10403, 10264);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10442, 10578);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10747, 10614);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 10779, 10909);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 11062, 10937);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 11084, 11203);
    AddPointsToBuilder(onSurfacePoints, builder, cutter, torsoOffset, 11218, 11204);

    const alg::Vec2& lastInternalPoint = onSurfacePoints[11204];

    const auto boundaryPoints = BoundaryPointsFromInternalPoint(lastInternalPoint, boundary);

    for (auto const& point : boundaryPoints)
        builder.AddPosition(GlobalPosition(torsoOffset, point, cutter));

    // Last position
    auto lastPos = builder.GetLastPosition();
    lastPos.SetY(millingSettings.safeHeight);
    builder.AddPosition(lastPos);

    std::cout << "Total points: " << builder.GetCommandsCnt() << std::endl;

    coordinator.DestroyEntity(torsoOffset);
    coordinator.DestroyEntity(upperFinOffset);
    coordinator.DestroyEntity(rightFinOffset);
    coordinator.DestroyEntity(leftFinOffset);
    coordinator.DestroyEntity(rightEyeOffset);
    coordinator.DestroyEntity(leftEyeOffset);
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForTorso(const MillingCutter& cutter, const std::unordered_map<std::string, Entity>& offsetSurfaces)
{
    const Entity baseOffset = c0PatchesSystem->CreatePlane(
        alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness + cutter.radius, -materialParameters.zLen/2.f),
        alg::Vec3::UnitY(),
        materialParameters.xLen, materialParameters.zLen
    );

    const Entity torsoOffset = offsetSurfaces.at("TorsoOffset");

    const Entity torsoRightFinInter = intersectionSystem->FindIntersection(torsoOffset, offsetSurfaces.at("RightFinOffset"), 1e-3).value();
    const Entity torsoLeftFinInter = intersectionSystem->FindIntersection(torsoOffset, offsetSurfaces.at("LeftFinOffset"), 1e-3).value();
    const Entity torsoBaseInter = intersectionSystem->FindIntersection(torsoOffset, baseOffset, 1e-3).value();

    auto const& torsoRightFinCurve = coordinator.GetComponent<IntersectionCurve>(torsoRightFinInter);
    auto const& torsoLeftFinCurve = coordinator.GetComponent<IntersectionCurve>(torsoLeftFinInter);
    auto const& torsoBaseCurve = coordinator.GetComponent<IntersectionCurve>(torsoBaseInter);

    auto torsoRightFinCurveNorm = GetPointsVec(torsoRightFinCurve);
    auto torsoLeftFinCurveNorm = GetPointsVec(torsoLeftFinCurve);
    auto torsoBaseCurveNorm = GetPointsVec(torsoBaseCurve);

    CircularVecWrap torsoRightFinCurveWrap(torsoRightFinCurveNorm);
    CircularVecWrap torsoLeftFinCurveWrap(torsoLeftFinCurveNorm);
    CircularVecWrap torsoBaseCurveWrap(torsoBaseCurveNorm);

    std::vector<alg::Vec2> boundary;

    int maxUBaseIdx = 0;
    float maxUBase = -std::numeric_limits<float>::infinity();
    for (int i = 0; i < torsoBaseCurveNorm.size(); ++i) {
        if (torsoBaseCurveNorm[i].X() > maxUBase) {
            maxUBase = torsoBaseCurveNorm[i].X();
            maxUBaseIdx = i;
        }
    }

    const int initBaseIdx = maxUBaseIdx;

    const int baseChange = torsoBaseCurveWrap[initBaseIdx+1].Y() > torsoBaseCurveWrap[initBaseIdx].Y() ? 1 : -1;
    int actBaseIdx = maxUBaseIdx + baseChange;
    bool intersectionFound = false;

    int actRightFinIdx = 0;

    boundary.emplace_back(torsoBaseCurveWrap[initBaseIdx]);

    while (!intersectionFound) {
        const auto& actBasePoint = torsoBaseCurveWrap[actBaseIdx];
        const auto& prevBasePoint = torsoBaseCurveWrap[actBaseIdx-baseChange];

        LineSegment2D baseSeg(actBasePoint, prevBasePoint);

        for (int i = 1; i < torsoRightFinCurveNorm.size(); ++i) {
            const auto actRightFinPoint = torsoRightFinCurveNorm[i];
            const auto prevRightFinPoint = torsoRightFinCurveNorm[i-1];

            LineSegment2D rightFinSeg(actRightFinPoint, prevRightFinPoint);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(baseSeg, rightFinSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                actRightFinIdx = i;
                break;
            }
        }

        if (!intersectionFound) {
            boundary.emplace_back(actBasePoint);
            actBaseIdx += baseChange;
        }
    }

    const int rightFinChange = torsoRightFinCurveNorm[actRightFinIdx+1].Y() < torsoRightFinCurveNorm[actRightFinIdx].Y() ? 1 : -1;
    actRightFinIdx += rightFinChange;

    intersectionFound = false;

    while (!intersectionFound) {
        const auto& actRightFinPoint = torsoRightFinCurveWrap[actRightFinIdx];
        const auto& prevRightFinPoint = torsoRightFinCurveWrap[actRightFinIdx-rightFinChange];

        LineSegment2D rightFinSeg(actRightFinPoint, prevRightFinPoint);

        for (int i=1; i < torsoBaseCurveNorm.size(); ++i) {
            const auto& actTorsoSeg = torsoBaseCurveNorm[i];
            const auto& prevTorsoSeg = torsoBaseCurveNorm[i-1];

            LineSegment2D torsoSeg(actTorsoSeg, prevTorsoSeg);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(rightFinSeg, torsoSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                actBaseIdx = i;
                break;
            }
        }

        if (!intersectionFound) {
            boundary.emplace_back(prevRightFinPoint);
            actRightFinIdx += rightFinChange;
        }
    }

    intersectionFound = false;
    int actLeftFinIdx = 0;

    while (!intersectionFound) {
        auto const& actBasePoint = torsoBaseCurveWrap[actBaseIdx];
        auto const& prevBasePoint = torsoBaseCurveWrap[actBaseIdx-baseChange];

        LineSegment2D baseSeg(actBasePoint, prevBasePoint);

        for (int i=1; i < torsoLeftFinCurveNorm.size(); ++i) {
            const auto& actLeftFinPoint = torsoLeftFinCurveNorm[i];
            const auto& prevLeftFinPoint = torsoLeftFinCurveNorm[i-1];

            LineSegment2D leftFinSeg(actLeftFinPoint, prevLeftFinPoint);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(baseSeg, leftFinSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                actLeftFinIdx = i;
                break;
            }
        }

        if (!intersectionFound) {
            boundary.emplace_back(actBasePoint);
            actBaseIdx += baseChange;
        }
    }

    intersectionFound = false;
    const int leftFinChange = torsoLeftFinCurveNorm[actLeftFinIdx+1].Y() > torsoLeftFinCurveNorm[actLeftFinIdx].Y() ? 1 : -1;
    actLeftFinIdx += leftFinChange;

    while (!intersectionFound) {
        const auto& actLeftFinPoint = torsoLeftFinCurveWrap[actLeftFinIdx];
        const auto& prevLeftFinPoint = torsoLeftFinCurveWrap[actLeftFinIdx-leftFinChange];

        LineSegment2D leftFinSeg(actLeftFinPoint, prevLeftFinPoint);

        for (int i=1; i < torsoBaseCurveNorm.size(); ++i) {
            const auto& actTorsoSeg = torsoBaseCurveNorm[i];
            const auto& prevTorsoSeg = torsoBaseCurveNorm[i-1];

            LineSegment2D torsoSeg(actTorsoSeg, prevTorsoSeg);

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(leftFinSeg, torsoSeg, interPoint)) {
                boundary.emplace_back(interPoint);
                intersectionFound = true;
                actBaseIdx = i;
                break;
            }
        }

        if (!intersectionFound) {
            boundary.emplace_back(prevLeftFinPoint);
            actLeftFinIdx += leftFinChange;
        }
    }

    while (actBaseIdx != initBaseIdx) {
        boundary.emplace_back(torsoBaseCurveWrap[actBaseIdx]);
        actBaseIdx += baseChange;
    }

    // InterCurveToFile("torsoRightFinIntersections.csv", torsoRightFinCurve);
    // InterCurveToFile("torsoLeftFinIntersections.csv", torsoLeftFinCurve);
    // InterCurveToFile("torsoBaseIntersections.csv", torsoBaseCurve);
    //
    // InterCurveToFile("torsoBoundary.csv", boundary);

    Update();
    coordinator.DestroyEntity(baseOffset);
    coordinator.DestroyEntity(torsoRightFinInter);
    coordinator.DestroyEntity(torsoLeftFinInter);
    coordinator.DestroyEntity(torsoBaseInter);

    return boundary;
}


std::vector<alg::Vec2> MillingPathsDesigner::GetBoundaryCurveForTorsoUpperFinIntersection(
    const MillingCutter &cutter, const std::unordered_map<std::string, Entity>& offsetSurfaces)
{
    const Entity upperFin = offsetSurfaces.at("UpperFin");
    const Entity upperFinOffset = offsetSurfaces.at("UpperFinOffset");
    const Entity torsoOffset = offsetSurfaces.at("TorsoOffset");

    coordinator.DeleteComponent<WrapV>(upperFinOffset);
    coordinator.DeleteComponent<WrapV>(upperFin);

    const Entity torsoUpperFinInter = intersectionSystem->FindIntersection(torsoOffset, upperFinOffset, 1e-3).value();
    auto const& torsoUpperFinCurve = coordinator.GetComponent<IntersectionCurve>(torsoUpperFinInter);

    coordinator.AddComponent<WrapV>(upperFinOffset, WrapV());
    coordinator.AddComponent<WrapV>(upperFin, WrapV());

    std::vector<alg::Vec2> torsoFinCurveNorm = GetPointsVec(torsoUpperFinCurve);

    InterCurveToFile("torsoUpperFinIntersections.csv", torsoUpperFinCurve);

    const auto heightMap = GenerateHeightMap(2000, 2000);

    const auto& firstCurvePoint = torsoFinCurveNorm.front();
    const auto& lastCurvePoint = torsoFinCurveNorm.back();

    const float undercutStartU = std::min(firstCurvePoint.X(), lastCurvePoint.X());
    constexpr float undercutEndU = 10.4f;
    const float undercutStartV = std::max(firstCurvePoint.Y(), lastCurvePoint.Y());
    const float undercutEndV = std::min(firstCurvePoint.Y(), lastCurvePoint.Y());

    constexpr float stepU = 0.005f;
    constexpr float stepV = 0.005f;

    const int uStepsCnt = static_cast<int>(std::floor((undercutEndU - undercutStartU) / stepU));
    const int vStepsCnt = static_cast<int>(std::floor((undercutStartV - undercutEndV) / stepV));

    const float offsetV = (undercutStartV - undercutEndV - static_cast<float>(vStepsCnt-1) * stepV) / 2.f;

    FlatVec2D<float> undercutMap(uStepsCnt, vStepsCnt);
    std::vector<alg::Vec2> undercutPoints;
    std::vector<alg::Vec2> noUndercutPoints;

    for (int actStepU=0; actStepU < uStepsCnt; actStepU++) {
        for (int actStepV=0; actStepV < vStepsCnt; actStepV++) {
            const float u = undercutStartU + static_cast<float>(actStepU) * stepU;
            const float v = undercutStartV - static_cast<float>(actStepV) * stepV - offsetV;

            Position pos = equidistanceC2System->PointOnSurface(torsoOffset, u, v);
            pos.vec.Y() -= cutter.radius;

            float heightMapY = MinYCutterPos(heightMap, cutter, pos.GetX(), pos.GetZ());

            if (pos.GetY() < heightMapY && heightMapY - pos.GetY() > 1e-4f) {
                undercutMap.At(actStepU, actStepV) = heightMapY - pos.GetY();
                undercutPoints.emplace_back(u, v);
            }
            else {
                undercutMap.At(actStepU, actStepV) = 0;
                noUndercutPoints.emplace_back(u, v);
            }
        }
    }

    for (int actStepV=0; actStepV < vStepsCnt; actStepV++) {
        for (int actStepU=0; actStepU < uStepsCnt; actStepU++) {
            if (undercutMap.At(actStepU, actStepV) == 0) {
                const float u = undercutStartU + static_cast<float>(actStepU) * stepU;
                const float v = undercutStartV - static_cast<float>(actStepV) * stepV - offsetV;

                torsoFinCurveNorm.emplace_back(u, v);
                break;
            }
        }
    }

    // InterCurveToFile("torsoFinCurveNorm.csv", torsoFinCurveNorm);

    // InterCurveToFile("undercutPoints.csv", undercutPoints);
    // InterCurveToFile("noUndercutPoints.csv", noUndercutPoints);
    //
    // Vector2DToCSV("undercutMap.csv", undercutMap);

    return torsoFinCurveNorm;
}


std::vector<std::vector<alg::Vec2>> MillingPathsDesigner::GetTorsoHoles(
    const MillingCutter &cutter, const std::unordered_map<std::string, Entity> &offsetSurfaces
) {
    const Entity torsoOffset = offsetSurfaces.at("TorsoOffset");

    const Entity torsoRightEyeInter = intersectionSystem->FindIntersection(torsoOffset, offsetSurfaces.at("RightEyeOffset"), 1e-3).value();
    const Entity torsoLeftEyeInter = intersectionSystem->FindIntersection(torsoOffset, offsetSurfaces.at("LeftEyeOffset"), 1e-3).value();

    auto const& torsoRightEyeCurve = coordinator.GetComponent<IntersectionCurve>(torsoRightEyeInter);
    auto const& torsoLeftEyeCurve = coordinator.GetComponent<IntersectionCurve>(torsoLeftEyeInter);

    auto torsoRightEyeCurveNorm = GetPointsVec(torsoRightEyeCurve);
    auto torsoLeftEyeCurveNorm = GetPointsVec(torsoLeftEyeCurve);

    auto torsoUpperFinCurve = GetBoundaryCurveForTorsoUpperFinIntersection(cutter, offsetSurfaces);

    // InterCurveToFile("torsoRightEyeIntersections.csv", torsoRightEyeCurve);
    // InterCurveToFile("torsoLeftEyeIntersections.csv", torsoLeftEyeCurve);

    coordinator.DestroyEntity(torsoRightEyeInter);
    coordinator.DestroyEntity(torsoLeftEyeInter);

    return {torsoRightEyeCurveNorm, torsoLeftEyeCurveNorm, torsoUpperFinCurve};
}
