#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>

#include <CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>
#include <CAD_modeler/model/millingPathsDesigner/millingMachinePathsBuilder.hpp>
#include <CAD_modeler/model/millingPathsDesigner/boundaryIntersectionFinder.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesTrianglesRenderSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesTrianglesRenderingSystem.hpp>
#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>
#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>
#include <CAD_modeler/model/systems/equidistanceC2SurfaceSystem.hpp>

#include <CAD_modeler/utilities/lineSegment2D.hpp>

#include <algorithm>


MillingPathsDesigner::MillingPathsDesigner(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
    // TODO: remove
    RegisterAllComponents(coordinator);

    GridSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    C0PatchesSystem::RegisterSystem(coordinator);
    C0PatchesRenderSystem::RegisterSystem(coordinator);
    C0PatchesTrianglesRenderSystem::RegisterSystem(coordinator);
    C2PatchesSystem::RegisterSystem(coordinator);
    C2PatchesRenderSystem::RegisterSystem(coordinator);
    C2PatchesTrianglesRenderSystem::RegisterSystem(coordinator);
    NameSystem::RegisterSystem(coordinator);
    SelectionSystem::RegisterSystem(coordinator);
    EquidistanceC2System::RegisterSystem(coordinator);
    IntersectionSystem::RegisterSystem(coordinator);
    InterpolationCurvesRenderingSystem::RegisterSystem(coordinator);
    PolylineSystem::RegisterSystem(coordinator);

    gridSystem = coordinator.GetSystem<GridSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c0PatchesRenderSystem = coordinator.GetSystem<C0PatchesRenderSystem>();
    c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    c2PatchesRenderSystem = coordinator.GetSystem<C2PatchesRenderSystem>();
    equidistanceC2System = coordinator.GetSystem<EquidistanceC2System>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    const auto selectionSys = coordinator.GetSystem<SelectionSystem>();
    const auto equidistanceSurfaceSys = coordinator.GetSystem<EquidistanceC2System>();
    intersectionSystem = coordinator.GetSystem<IntersectionSystem>();
    interpolationCurvesRendering = coordinator.GetSystem<InterpolationCurvesRenderingSystem>();
    polylineSystem = coordinator.GetSystem<PolylineSystem>();

    gridSystem->Init();
    c0PatchesSystem->Init();
    c2PatchesSystem->Init();
    equidistanceC2System->Init();
    selectionSys->Init();
    equidistanceSurfaceSys->Init();

    base = c0PatchesSystem->CreatePlane(
    alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness, -materialParameters.zLen/2.f),
    alg::Vec3::UnitY(),
    materialParameters.xLen, materialParameters.zLen
    );
    coordinator.AddComponent<DrawStd>(base, DrawStd());

    LoadModel("../../models/fish_model.json");

    // Created base plane must be updated
    Update();
}


void MillingPathsDesigner::LoadModel(const std::string &filePath)
{
    LoadManager loadManager;
    loadManager.Load(filePath, coordinator, LoadManager::C0Surfaces | LoadManager::C2Surfaces);

    // Raise loaded items
    const auto cpRegistrySystem = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (const auto point: pointsSystem->GetEntities()) {
        if (cpRegistrySystem->IsAControlPoint(base, point, Coordinator::GetSystemID<C0PatchesSystem>()))
            continue;

        coordinator.EditComponent<Position>(point,
            [this](Position& pos) {
                pos.SetY(pos.GetY() + millingSettings.baseThickness);
            }
        );
    }
}


void MillingPathsDesigner::Update()
{
    c0PatchesSystem->Update();
    c2PatchesSystem->Update();
    interpolationCurvesRendering->Update();
}


void MillingPathsDesigner::GenerateBroadPhase()
{
    const MillingCutter cutter(0.08, MillingCutter::Type::Round);
    MillingMachinePathsBuilder builder;

    auto heightMap = GenerateBroadPhaseHeightMap();

    const float toMill = materialParameters.yLen - millingSettings.baseThickness + millingSettings.broadPhaseAdditionalThickness;
    const float toMillHalf = toMill / 2.f;

    heightMap.defaultHeight = materialParameters.yLen - toMillHalf;

    float minXCutterPos = heightMap.MinX() - cutter.radius * 1.5f;
    float minZCutterPos = heightMap.MinZ() - cutter.radius * 1.5f;

    float maxXCutterPos = heightMap.MaxX() + cutter.radius * 1.5f;
    float maxZCutterPos = heightMap.MaxZ() + cutter.radius * 1.5f;

    std::tie(minXCutterPos, minZCutterPos) = heightMap.NearestPixelPoint(minXCutterPos, minZCutterPos);
    std::tie(maxXCutterPos, maxZCutterPos) = heightMap.NearestPixelPoint(maxXCutterPos, maxZCutterPos);

    // First position
    builder.AddPosition(millingSettings.initCutterPos);

    // Second position
    builder.AddPosition(Position(
        minXCutterPos,
        millingSettings.initCutterPos.Y(),
        minZCutterPos
    ));

    // Third position
    builder.AddPosition(Position(
        minXCutterPos,
        heightMap.defaultHeight,
        minZCutterPos
    ));

    const float stepLenInXDir = cutter.radius * 1.5f;

    const int stepsInXDir = static_cast<int>(std::ceil((maxXCutterPos - minXCutterPos) / stepLenInXDir));
    const int stepsInZDir = static_cast<int>(std::ceil((maxZCutterPos - minZCutterPos) / heightMap.PixelXLen()));

    for (int stepX=0; stepX < stepsInXDir+1; stepX++) {
        const float actCutterMiddleX = minXCutterPos + static_cast<float>(stepX) * stepLenInXDir;

        for (int stepZ=0; stepZ < stepsInZDir+1; stepZ++) {
            float actCutterMiddleZ;

            if (stepX % 2 == 0)
                actCutterMiddleZ = minZCutterPos + static_cast<float>(stepZ) * heightMap.PixelZLen();
            else
                actCutterMiddleZ = maxZCutterPos - static_cast<float>(stepZ) * heightMap.PixelZLen();

            const float minCutterY = MinYCutterPos(heightMap, cutter, actCutterMiddleX, actCutterMiddleZ);

            Position newCutterPos(actCutterMiddleX, minCutterY, actCutterMiddleZ);
            builder.AddPosition(newCutterPos);
        }

        if (stepX < stepsInXDir)
            builder.AddPositionFromOffset(alg::Vec3::UnitX() * stepLenInXDir);
    }

    heightMap.defaultHeight = millingSettings.baseThickness + millingSettings.broadPhaseAdditionalThickness;

    auto prevPos = builder.GetLastPosition();
    prevPos.SetY(heightMap.defaultHeight);
    builder.AddPosition(prevPos);

    for (int stepX=0; stepX < stepsInXDir+1; stepX++) {
        const float actCutterMiddleX = minXCutterPos + static_cast<float>(stepsInXDir - stepX) * stepLenInXDir;

        for (int stepZ=0; stepZ < stepsInZDir+1; stepZ++) {
            float actCutterMiddleZ;

            if ((stepX+stepsInXDir) % 2 == 1)
                actCutterMiddleZ = minZCutterPos + static_cast<float>(stepZ) * heightMap.PixelZLen();
            else
                actCutterMiddleZ = maxZCutterPos - static_cast<float>(stepZ) * heightMap.PixelZLen();

            const float minCutterY = MinYCutterPos(heightMap, cutter, actCutterMiddleX, actCutterMiddleZ);

            Position newCutterPos(actCutterMiddleX, minCutterY, actCutterMiddleZ);
            builder.AddPosition(newCutterPos);
        }

        if (stepX < stepsInXDir)
            builder.AddPositionFromOffset(-alg::Vec3::UnitX() * stepLenInXDir);
    }

    prevPos = builder.GetLastPosition();
    prevPos.SetY(millingSettings.initCutterPos.Y());
    builder.AddPosition(prevPos);

    builder.AddPosition(millingSettings.initCutterPos);

    MillingMachinePathsSystem::CreateGCodeFile(builder.GetPaths(), "paths/1.k16");
}


void MillingPathsDesigner::GenerateBasePhase()
{
    const MillingCutter cutter(0.05, MillingCutter::Type::Flat);
    MillingMachinePathsBuilder builder;

    const auto step1Boundary = FindBoundary(cutter.radius * 1.5f);

    float cutterMaxZPos = materialParameters.zLen / 2.f + cutter.radius * 1.5f;
    float cutterMinZPos = -cutterMaxZPos;

    const float xStepLen = 2.f * cutter.radius - 0.1f * cutter.radius;

    float minXBoundary = step1Boundary.front().GetX();
    float maxXBoundary = -std::numeric_limits<float>::infinity();
    for (const auto& point: step1Boundary)
        if (point.GetX() > maxXBoundary)
            maxXBoundary = point.GetX();

    float materialMinX = -materialParameters.xLen / 2.f;

    float materialBorderToBoundary = minXBoundary - materialMinX;

    int initFullSteps = static_cast<int>(std::ceil(materialBorderToBoundary / xStepLen));
    int cutterXSteps = static_cast<int>(std::ceil(materialParameters.xLen / xStepLen));

    float initCutterX = minXBoundary - (initFullSteps - 1) * xStepLen;

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
        float xCoord = initCutterX + stepsDone * xStepLen;

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
    float actX = initCutterX + stepsDone * xStepLen;
    while (actX + xStepLen < maxXBoundary) {
        stepsDone++;
        actX = initCutterX + stepsDone * xStepLen;

        builder.AddPosition(actX, millingSettings.baseThickness, cutterMaxZPos);
        builder.AddPosition(finder.Intersection(actX));

        if (actX + xStepLen > maxXBoundary)
            break;

        stepsDone++;
        actX = initCutterX + stepsDone * xStepLen;

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
        actX = initCutterX + stepsDone * xStepLen;

        builder.AddPosition(actX, millingSettings.baseThickness, cutterMinZPos);
        builder.AddPosition(finder.Intersection(actX));

        if (actX - xStepLen < minXBoundary)
            break;

        if (stepsDone == 7) {
            builder.AddPosition(finder.Intersection(actX - xStepLen/2.f));
        }

        stepsDone--;
        actX = initCutterX + stepsDone * xStepLen;

        builder.AddPosition(finder.Intersection(actX));
        builder.AddPosition(actX, millingSettings.baseThickness, cutterMinZPos);
    }

    builder.PopLastPosition();

    const auto step2Boundary = FindBoundary(cutter.radius);

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


void MillingPathsDesigner::GenerateMainPhase()
{
    const MillingCutter cutter(0.04, MillingCutter::Type::Flat);

    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity torsoOffset = equidistanceC2System->AddSurface(torso, -cutter.radius);

    const Entity pletwa_prawa = nameSystem->EntityFromName("pletwa_prawa");
    const Entity pletwa_prawaOffset = equidistanceC2System->AddSurface(pletwa_prawa, -cutter.radius);

    const Entity pletwa_lewa = nameSystem->EntityFromName("pletwa_lewa");
    const Entity pletwa_lewaOffset = equidistanceC2System->AddSurface(pletwa_lewa, -cutter.radius);

    // const Entity pletwa_gorna = nameSystem->EntityFromName("pletwa_gorna");
    // const Entity pletwa_gornaOffset = equidistanceC2System->AddSurface(pletwa_gorna, -cutter.radius);

    const Entity prawe_oko = nameSystem->EntityFromName("prawe_oko");
    const Entity prawe_okoOffset = equidistanceC2System->AddSurface(prawe_oko, -cutter.radius);

    const Entity lewe_oko = nameSystem->EntityFromName("lewe_oko");
    const Entity lewe_okoOffset = equidistanceC2System->AddSurface(lewe_oko, -cutter.radius);

    const Entity baseOffset = c0PatchesSystem->CreatePlane(
        alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness + cutter.radius, -materialParameters.zLen/2.f),
        alg::Vec3::UnitY(),
        materialParameters.xLen, materialParameters.zLen
    );

    intersectionSystem->FindIntersection(pletwa_prawaOffset, torsoOffset, 1e-3);
    intersectionSystem->FindIntersection(pletwa_lewaOffset, torsoOffset, 1e-3);
    //intersectionSystem->FindIntersection(pletwa_gornaOffset, torsoOffset, 1e-3);

    intersectionSystem->FindIntersection(prawe_okoOffset, torsoOffset, 1e-3);
    intersectionSystem->FindIntersection(lewe_okoOffset, torsoOffset, 1e-3);

    intersectionSystem->FindIntersection(baseOffset, torsoOffset, 1e-3);
    intersectionSystem->FindIntersection(pletwa_prawaOffset, baseOffset, 1e-3);
    intersectionSystem->FindIntersection(pletwa_lewaOffset, baseOffset, 1e-3);
}


void MillingPathsDesigner::RenderSystemsObjects(
    const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, const float nearPlane, const float farPlane) const
{
    const alg::Mat4x4 cameraMtx = persMtx * viewMtx;

    c0PatchesRenderSystem->Render(cameraMtx);
    c2PatchesRenderSystem->Render(cameraMtx);
    interpolationCurvesRendering->Render(cameraMtx);
    polylineSystem->Render(cameraMtx);

    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
}


// TODO: remove
#include <CAD_modeler/utilities/toPGM.hpp>


BroadPhaseHeightMap MillingPathsDesigner::GenerateBroadPhaseHeightMap()
{
    constexpr int depthBufferResolution = 300;
    const DepthBuffer depthBuffer(depthBufferResolution, depthBufferResolution);

    auto const& c0Renderer = coordinator.GetSystem<C0PatchesTrianglesRenderSystem>();
    auto const& c2Renderer = coordinator.GetSystem<C2PatchesTrianglesRenderSystem>();

    for (const auto entity: c0PatchesSystem->GetEntities()) {
        if (entity == base) {
            c0Renderer->AddSurface(entity, 1.f);
            continue;
        }

        c0Renderer->AddSurface(entity);
    }

    for (const auto entity: c2PatchesSystem->GetEntities())
        c2Renderer->AddSurface(entity);

    depthBuffer.Use();

    auto [oldViewportWidth, oldViewportHeight] = GetViewportSize();
    ChangeViewportSize(depthBufferResolution, depthBufferResolution);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDrawBuffer(GL_NONE);
    // glReadBuffer(GL_NONE);
    glDepthMask(GL_TRUE);

    const alg::Vec3 camPos(0.f, materialParameters.yLen, 0.f);

    const alg::Mat4x4 viewMatrix = alg::LookAt(camPos, alg::Vec3::UnitY(), alg::Vec3::UnitX());
    const alg::Mat4x4 projectionMtx = alg::OrthographicProjection(
        -materialParameters.xLen/2.0f,
        materialParameters.xLen/2.0f,
        materialParameters.zLen/2.0f,
        -materialParameters.zLen/2.0f,
        0.f,
        materialParameters.yLen
    );

    const alg::Mat4x4 camMtx = projectionMtx * viewMatrix;

    c0Renderer->Render(camMtx);
    c2Renderer->Render(camMtx);

    BroadPhaseHeightMap heightMap(
        depthBufferResolution,
        depthBufferResolution,
        materialParameters.xLen,
        materialParameters.zLen
    );
    glReadPixels(0, 0, depthBufferResolution, depthBufferResolution, GL_DEPTH_COMPONENT, GL_FLOAT, heightMap.Data());

    float near = 0.f;
    float far = materialParameters.yLen;
    std::ranges::for_each(heightMap, [near, far](float& d) {
        d = far - ((2.0f * d - 1.0f) * (far - near) + (far + near)) * 0.5f;
    });

    std::ranges::for_each(heightMap, [this](float& d) {
        d += this->millingSettings.broadPhaseAdditionalThickness;
    });

    ToPGM(heightMap.ToFlatVec2D(), "depth.pgm");

    DepthBuffer::UseDefault();
    ChangeViewportSize(oldViewportWidth, oldViewportHeight);

    // glDrawBuffer(GL_BACK);
    // glReadBuffer(GL_BACK);
    glDepthMask(GL_FALSE);

    return heightMap;
}


float MillingPathsDesigner::MinYCutterPos(
    const BroadPhaseHeightMap &heightMap, const MillingCutter &cutter, const float cutterX, const float cutterZ) const
{
    const int cutterXLenInPixels = static_cast<int>(std::ceil(cutter.radius / heightMap.PixelXLen() * 2.f));
    const int cutterZLenInPixels = static_cast<int>(std::ceil(cutter.radius / heightMap.PixelZLen() * 2.f));

    float maxCutterYCoord = -std::numeric_limits<float>::infinity();
    for (int sampleX=0; sampleX < cutterXLenInPixels; sampleX++) {
        for (int sampleZ=0; sampleZ < cutterZLenInPixels; sampleZ++) {
            const float sampleXPos = cutterX - cutter.radius + static_cast<float>(sampleX) * heightMap.PixelXLen();
            const float sampleZPos = cutterZ - cutter.radius + static_cast<float>(sampleZ) * heightMap.PixelZLen();

            const float sampleYPos = heightMap.HeightFromGlobalCoordinates(sampleXPos, sampleZPos);

            Position samplePos(sampleXPos, sampleYPos, sampleZPos);

            const float cutterY = cutter.CalcMinYCoord(cutterX, cutterZ, samplePos);
            if (cutterY > maxCutterYCoord)
                maxCutterYCoord = cutterY;
        }
    }

    return maxCutterYCoord;
}


std::vector<Position> MillingPathsDesigner::FindBoundary(float dist)
{
    const Entity torso = nameSystem->EntityFromName("torso");
    const Entity rightFin = nameSystem->EntityFromName("right fin");
    const Entity leftFin = nameSystem->EntityFromName("left fin");

    const auto torsoPoints = BoundaryPoints(torso, dist);
    const auto rightFinPoints = BoundaryPoints(rightFin, dist);
    const auto leftFinPoints = BoundaryPoints(leftFin, dist);

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

            if (LineSegment2D::AreIntersecting(torsoSeg, finSeg)) {
                interFound = true;
                auto interPoint = LineSegment2D::IntersectionPoint(torsoSeg, finSeg).value();

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

            if (LineSegment2D::AreIntersecting(finSeg, torsoSeg)) {
                interFound = true;
                auto interPoint = LineSegment2D::IntersectionPoint(torsoSeg, finSeg).value();

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

            if (LineSegment2D::AreIntersecting(torsoSeg, finSeg)) {
                interFound = true;
                auto interPoint = LineSegment2D::IntersectionPoint(torsoSeg, finSeg).value();

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

            if (LineSegment2D::AreIntersecting(finSeg, torsoSeg)) {
                interFound = true;
                auto interPoint = LineSegment2D::IntersectionPoint(torsoSeg, finSeg).value();

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


CircularVector<Position> MillingPathsDesigner::BoundaryPoints(const Entity entity, const float dist)
{
    const auto intersectionEntity = intersectionSystem->FindIntersection(entity, base, 1e-3);
    if (!intersectionEntity.has_value())
        throw std::runtime_error("No intersection found with base");

    auto const& c2Patches = coordinator.GetComponent<C2Patches>(entity);
    auto const& curve = coordinator.GetComponent<IntersectionCurve>(intersectionEntity.value());

    std::vector<Position> result(curve.Size());

    for (size_t i=0; i < curve.Size(); ++i)
        result[i] = BoundaryPoint(curve[i], c2Patches, dist);

    coordinator.DestroyEntity(intersectionEntity.value());

    return CircularVector(std::move(result));
}


Position MillingPathsDesigner::BoundaryPoint(const IntersectionPoint &p, const C2Patches& patches, const float dist) const
{
    const float u = p.U1();
    const float v = p.V1();

    const alg::Vec3 normal = c2PatchesSystem->NormalVector(patches, u, v);
    const alg::Vec3 normalProjection = alg::Vec3::UnitZ() * Dot(normal, alg::Vec3::UnitZ()) + alg::Vec3::UnitX() * Dot(normal, alg::Vec3::UnitX());

    alg::Vec3 result = c2PatchesSystem->PointOnSurface(patches, u, v).vec + dist * normalProjection.Normalize();
    result.Y() = millingSettings.baseThickness;

    return { result };
}
