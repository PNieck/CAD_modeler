#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/components/wraps.hpp>

#include "CAD_modeler/utilities/toFile.hpp"
#include "CAD_modeler/utilities/circularVecWrap.hpp"
#include "CAD_modeler/utilities/lineSegment2D.hpp"


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

    GetBoundaryCurveForTorso(cutter, offsetSurfaces);
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

    const LineSegment2D stitchLine(0.f, 3.f, 15.f, 3.f);

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

    InterCurveToFile("torsoRightEyeIntersections.csv", torsoRightEyeCurve);
    InterCurveToFile("torsoLeftEyeIntersections.csv", torsoLeftEyeCurve);

    coordinator.DestroyEntity(torsoRightEyeInter);
    coordinator.DestroyEntity(torsoLeftEyeInter);

    return {torsoRightEyeCurveNorm, torsoLeftEyeCurveNorm, torsoUpperFinCurve};
}
