#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include "CAD_modeler/model/millingPathsDesigner/boundaryIntersectionFinder.hpp"

#include "CAD_modeler/utilities/lineSegment2D.hpp"

#include "CAD_modeler/model/systems/millingMachinePathsSystem.hpp"


void MillingPathsDesigner::GenerateBasePhase()
{
    const MillingCutter cutter(0.05, MillingCutter::Type::Flat);
    MillingMachinePathsBuilder builder;

    const auto step1Boundary = FindModelBoundary(-cutter.radius * 1.5f);

    const float cutterMaxZPos = materialParameters.zLen / 2.f + cutter.radius * 1.5f;
    const float cutterMinZPos = -cutterMaxZPos;

    const float xStepLen = 2.f * cutter.radius - 0.1f * cutter.radius;

    const float minXBoundary = step1Boundary.front().GetX();
    float maxXBoundary = -std::numeric_limits<float>::infinity();
    for (const auto& point: step1Boundary)
        if (point.GetX() > maxXBoundary)
            maxXBoundary = point.GetX();

    const float materialMinX = -materialParameters.xLen / 2.f;

    const float materialBorderToBoundary = minXBoundary - materialMinX;

    const int initFullSteps = static_cast<int>(std::ceil(materialBorderToBoundary / xStepLen));

    const float initCutterX = minXBoundary - static_cast<float>(initFullSteps - 1) * xStepLen;

    // First position
    builder.AddPosition(millingSettings.initCutterPos);

    float firstZ, secondZ;

    if (initFullSteps % 2 == 1) {
        firstZ = cutterMinZPos;
        secondZ = cutterMaxZPos;
    }
    else {
        firstZ = cutterMaxZPos;
        secondZ = cutterMinZPos;
    }

    builder.AddPosition(initCutterX, millingSettings.initCutterPos.Y(), firstZ);

    int stepsDone = 0;
    for (int i=0; i < initFullSteps; i++) {
        const float xCoord = initCutterX + static_cast<float>(stepsDone) * xStepLen;

        if (i % 2 == 0) {
            builder.AddPosition(xCoord, millingSettings.baseThickness, firstZ);
            builder.AddPosition(xCoord, millingSettings.baseThickness, secondZ);
        }
        else {
            builder.AddPosition(xCoord, millingSettings.baseThickness, secondZ);
            builder.AddPosition(xCoord, millingSettings.baseThickness, firstZ);
        }

        stepsDone++;
    }

    BoundaryIntersectionFinder finder(step1Boundary);

    stepsDone--;
    float actX = initCutterX + static_cast<float>(stepsDone) * xStepLen;
    while (actX + xStepLen < maxXBoundary) {
        stepsDone++;
        actX = initCutterX + static_cast<float>(stepsDone) * xStepLen;

        builder.AddPosition(actX, millingSettings.baseThickness, cutterMaxZPos);
        builder.AddPosition(finder.Intersection(actX));

        if (actX + xStepLen > maxXBoundary)
            break;

        stepsDone++;
        actX = initCutterX + static_cast<float>(stepsDone) * xStepLen;

        builder.AddPosition(finder.Intersection(actX));
        builder.AddPosition(actX, millingSettings.baseThickness, cutterMaxZPos);
    }

    builder.AddPosition(actX, millingSettings.baseThickness, cutterMaxZPos);
    actX = maxXBoundary;

    builder.AddPosition(actX, millingSettings.baseThickness, cutterMaxZPos);
    builder.AddPosition(actX, millingSettings.baseThickness, cutterMinZPos);

    stepsDone++;
    while (actX - xStepLen > minXBoundary) {
        stepsDone--;
        actX = initCutterX + static_cast<float>(stepsDone) * xStepLen;

        builder.AddPosition(actX, millingSettings.baseThickness, cutterMinZPos);
        builder.AddPosition(finder.Intersection(actX));

        if (actX - xStepLen < minXBoundary)
            break;

        if (stepsDone == 7) {
            builder.AddPosition(finder.Intersection(actX - xStepLen/2.f));
        }

        stepsDone--;
        actX = initCutterX + static_cast<float>(stepsDone) * xStepLen;

        builder.AddPosition(finder.Intersection(actX));
        builder.AddPosition(actX, millingSettings.baseThickness, cutterMinZPos);
    }

    const auto step2Boundary = FindModelBoundary(-cutter.radius);

    for (const auto& point: step2Boundary)
        builder.AddPosition(point);

    builder.AddPosition(step2Boundary.back().GetX(), millingSettings.initCutterPos.Y(), step2Boundary.back().GetZ());
    builder.AddPosition(millingSettings.initCutterPos);

    auto paths = builder.GetPaths();
    std::vector<Position> pathsPositions;
    pathsPositions.reserve(paths.Size());

    for (const auto& path: paths) {
        pathsPositions.push_back(path.destination);
    }

    polylineSystem->AddPolyline(pathsPositions);

    MillingMachinePathsSystem::CreateGCodeFile(paths, "paths/2.f10");
}


std::vector<Position> MillingPathsDesigner::FindModelBoundary(float dist)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity rightFin = nameSystem->EntityFromName("right fin");
    const Entity leftFin = nameSystem->EntityFromName("left fin");

    const auto torsoPoints = ModelBoundaryPoints(torso, dist);
    const auto rightFinPoints = ModelBoundaryPoints(rightFin, dist);
    const auto leftFinPoints = ModelBoundaryPoints(leftFin, dist);

    size_t minXTorsoIdx = 0;
    float minXTorso = std::numeric_limits<float>::infinity();

    for (size_t i = 0; i < torsoPoints.size(); ++i) {
        if (torsoPoints[i].GetX() < minXTorso) {
            minXTorso = torsoPoints[i].GetX();
            minXTorsoIdx = i;
        }
    }

    float maxXRightFin = -std::numeric_limits<float>::infinity();
    float minXRightFin = std::numeric_limits<float>::infinity();

    for (const auto& point: rightFinPoints) {
        if (point.GetX() > maxXRightFin)
            maxXRightFin = point.GetX();

        if (point.GetX() < minXRightFin)
            minXRightFin = point.GetX();
    }

    float maxXLeftFin = -std::numeric_limits<float>::infinity();
    float minXLeftFin = std::numeric_limits<float>::infinity();

    for (const auto& point: leftFinPoints) {
        if (point.GetX() > maxXLeftFin)
            maxXLeftFin = point.GetX();

        if (point.GetX() < minXLeftFin)
            minXLeftFin = point.GetX();
    }


    std::vector<Position> result;
    result.reserve(torsoPoints.size() + rightFinPoints.size() + leftFinPoints.size());

    int torsoIdx = static_cast<int>(minXTorsoIdx);
    const int torsoChange = torsoPoints[torsoIdx+1].GetZ() > torsoPoints[torsoIdx].GetZ() ? 1 : -1;

    while (torsoPoints[torsoIdx].GetX() < minXLeftFin) {
        result.emplace_back(torsoPoints[torsoIdx]);
        torsoIdx += torsoChange;
    }

    int leftFinChange = 0;
    int leftFinIdx = 0;

    bool interFound = false;
    while (!interFound) {
        const auto& lastTorsoPoint = result.back();
        const auto& nextTorsoPoint = torsoPoints[torsoIdx];
        LineSegment2D torsoSeg(lastTorsoPoint.GetX(), lastTorsoPoint.GetZ(), nextTorsoPoint.GetX(), nextTorsoPoint.GetZ());

        for (int j = 1; j < leftFinPoints.size(); ++j) {
            const auto& lastFinPoint = leftFinPoints[j-1];
            const auto& nextFinPoint = leftFinPoints[j];
            LineSegment2D finSeg(lastFinPoint.GetX(), lastFinPoint.GetZ(), nextFinPoint.GetX(), nextFinPoint.GetZ());

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(torsoSeg, finSeg, interPoint)) {
                interFound = true;

                result.emplace_back(interPoint.X(), millingSettings.baseThickness, interPoint.Y());
                result.emplace_back(nextFinPoint);

                leftFinIdx = j;
                leftFinChange = leftFinPoints[leftFinIdx+1].GetZ() > leftFinPoints[leftFinIdx].GetZ() ? 1 : -1;
                leftFinIdx += leftFinChange;

                break;
            }
        }

        if (!interFound) {
            result.emplace_back(nextTorsoPoint);
            torsoIdx += torsoChange;
        }
    }

    int torsoInterMinIdx = torsoIdx + torsoChange;
    int torsoInterMaxIdx = torsoInterMinIdx;

    while (torsoPoints[torsoInterMaxIdx].GetX() < maxXLeftFin)
        torsoInterMaxIdx += torsoChange;

    interFound = false;
    while (!interFound) {
        const auto& lastFinPoint = result.back();
        const auto& nextFinPoint = leftFinPoints[leftFinIdx];
        LineSegment2D finSeg(lastFinPoint.GetX(), lastFinPoint.GetZ(), nextFinPoint.GetX(), nextFinPoint.GetZ());

        for (int j = torsoInterMinIdx + torsoChange; j != torsoInterMaxIdx; j += torsoChange) {
            const auto& lastTorsoPoint = torsoPoints[j-torsoChange];
            const auto& nextTorsoPoint = torsoPoints[j];
            LineSegment2D torsoSeg(lastTorsoPoint.GetX(), lastTorsoPoint.GetZ(), nextTorsoPoint.GetX(), nextTorsoPoint.GetZ());

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(finSeg, torsoSeg, interPoint)) {
                interFound = true;

                result.emplace_back(interPoint.X(), millingSettings.baseThickness, interPoint.Y());
                result.emplace_back(nextTorsoPoint);

                torsoIdx = j + torsoChange;
            }
        }

        if (!interFound) {
            result.emplace_back(nextFinPoint);
            leftFinIdx += leftFinChange;
        }
    }

    while (torsoPoints[torsoIdx].GetX() < maxXRightFin) {
        result.emplace_back(torsoPoints[torsoIdx]);
        torsoIdx += torsoChange;
    }

    while (torsoPoints[torsoIdx].GetX() > maxXRightFin) {
        result.emplace_back(torsoPoints[torsoIdx]);
        torsoIdx += torsoChange;
    }

    int rightFinChange = 0;
    int rightFinIdx = 0;

    interFound = false;
    while (!interFound) {
        const auto& lastTorsoPoint = result.back();
        const auto& nextTorsoPoint = torsoPoints[torsoIdx];
        LineSegment2D torsoSeg(lastTorsoPoint.GetX(), lastTorsoPoint.GetZ(), nextTorsoPoint.GetX(), nextTorsoPoint.GetZ());

        for (int j = 1; j < static_cast<int>(rightFinPoints.size()); ++j) {
            const auto& lastFinPoint = rightFinPoints[j-1];
            const auto& nextFinPoint = rightFinPoints[j];
            LineSegment2D finSeg(lastFinPoint.GetX(), lastFinPoint.GetZ(), nextFinPoint.GetX(), nextFinPoint.GetZ());

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(torsoSeg, finSeg, interPoint)) {
                interFound = true;

                result.emplace_back(interPoint.X(), millingSettings.baseThickness, interPoint.Y());
                result.emplace_back(nextFinPoint);

                rightFinIdx = j;
                rightFinChange = rightFinPoints[leftFinIdx+1].GetZ() > rightFinPoints[leftFinIdx].GetZ() ? 1 : -1;
                rightFinIdx += rightFinChange;

                break;
            }
        }

        if (!interFound) {
            result.emplace_back(nextTorsoPoint);
            torsoIdx += torsoChange;
        }
    }

    torsoInterMinIdx = torsoIdx + torsoChange;
    torsoInterMaxIdx = torsoInterMinIdx;

    while (torsoPoints[torsoInterMaxIdx].GetX() > minXRightFin)
        torsoInterMaxIdx += torsoChange;

    interFound = false;
    while (!interFound) {
        const auto& lastFinPoint = result.back();
        const auto& nextFinPoint = rightFinPoints[rightFinIdx];
        LineSegment2D finSeg(lastFinPoint.GetX(), lastFinPoint.GetZ(), nextFinPoint.GetX(), nextFinPoint.GetZ());

        for (int j = torsoInterMinIdx + torsoChange; j != torsoInterMaxIdx; j += torsoChange) {
            const auto& lastTorsoPoint = torsoPoints[j-torsoChange];
            const auto& nextTorsoPoint = torsoPoints[j];
            LineSegment2D torsoSeg(lastTorsoPoint.GetX(), lastTorsoPoint.GetZ(), nextTorsoPoint.GetX(), nextTorsoPoint.GetZ());

            alg::Vec2 interPoint;
            if (LineSegment2D::AreIntersecting(finSeg, torsoSeg, interPoint)) {
                interFound = true;

                result.emplace_back(interPoint.X(), millingSettings.baseThickness, interPoint.Y());
                result.emplace_back(nextTorsoPoint);

                torsoIdx = j + torsoChange;
            }
        }

        if (!interFound) {
            result.emplace_back(nextFinPoint);
            rightFinIdx += rightFinChange;
        }
    }

    size_t startingTorsoIdx = torsoPoints.CircularIndex(minXTorsoIdx);
    while (torsoPoints.CircularIndex(torsoIdx) != startingTorsoIdx) {
        result.emplace_back(torsoPoints[torsoIdx]);
        torsoIdx += torsoChange;
    }

    result.emplace_back(torsoPoints[torsoIdx]);

    assert(result.front().vec == result.back().vec);

    polylineSystem->AddPolyline(result);

    return result;
}


CircularVector<Position> MillingPathsDesigner::ModelBoundaryPoints(const Entity entity, const float dist)
{
    const auto intersectionEntity = intersectionSystem->FindIntersection(entity, base, 1e-3);
    if (!intersectionEntity.has_value())
        throw std::runtime_error("No intersection found with base");

    auto const& c2Patches = coordinator.GetComponent<C2Patches>(entity);
    auto const& curve = coordinator.GetComponent<IntersectionCurve>(intersectionEntity.value());

    std::vector<Position> result(curve.Size());

    for (size_t i=0; i < curve.Size(); ++i)
        result[i] = ModelBoundaryPoint(curve[i], c2Patches, dist);

    coordinator.DestroyEntity(intersectionEntity.value());

    return CircularVector(std::move(result));
}


Position MillingPathsDesigner::ModelBoundaryPoint(const IntersectionPoint &p, const C2Patches& patches, const float dist) const
{
    const float u = p.U1();
    const float v = p.V1();

    const alg::Vec3 normal = c2PatchesSystem->NormalVector(patches, u, v);
    const alg::Vec3 normalProjection = alg::Vec3::UnitZ() * Dot(normal, alg::Vec3::UnitZ()) + alg::Vec3::UnitX() * Dot(normal, alg::Vec3::UnitX());

    alg::Vec3 result = c2PatchesSystem->PointOnSurface(patches, u, v).vec + dist * normalProjection.Normalize();
    result.Y() = millingSettings.baseThickness;

    return { result };
}
