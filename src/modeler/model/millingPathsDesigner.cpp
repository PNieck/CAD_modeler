#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>
#include "CAD_modeler/model/components/uvVisualization.hpp"
#include "CAD_modeler/model/components/wraps.hpp"

#include <CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>
#include <CAD_modeler/model/millingPathsDesigner/millingMachinePathsBuilder.hpp>
#include <CAD_modeler/model/millingPathsDesigner/boundaryIntersectionFinder.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesTrianglesRenderSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesTrianglesRenderingSystem.hpp>
#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>
#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>
#include <CAD_modeler/model/systems/equidistanceC2SurfaceSystem.hpp>
#include <CAD_modeler/model/systems/uvVisualizer.hpp>
#include <CAD_modeler/model/systems/utils/getSurfaceSystem.hpp>

#include <CAD_modeler/utilities/lineSegment2D.hpp>
#include <CAD_modeler/utilities/circularVecWrap.hpp>

#include <algorithm>
#include <fstream>


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
    EquidistanceC2SurfaceSystem::RegisterSystem(coordinator);
    EquidistanceC0SurfaceSystem::RegisterSystem(coordinator);
    IntersectionSystem::RegisterSystem(coordinator);
    InterpolationCurvesRenderingSystem::RegisterSystem(coordinator);
    PolylineSystem::RegisterSystem(coordinator);

    gridSystem = coordinator.GetSystem<GridSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c0PatchesRenderSystem = coordinator.GetSystem<C0PatchesRenderSystem>();
    c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    c2PatchesRenderSystem = coordinator.GetSystem<C2PatchesRenderSystem>();
    equidistanceC2System = coordinator.GetSystem<EquidistanceC2SurfaceSystem>();
    equidistanceC0System = coordinator.GetSystem<EquidistanceC0SurfaceSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    const auto selectionSys = coordinator.GetSystem<SelectionSystem>();
    const auto equidistanceSurfaceSys = coordinator.GetSystem<EquidistanceC2SurfaceSystem>();
    intersectionSystem = coordinator.GetSystem<IntersectionSystem>();
    interpolationCurvesRendering = coordinator.GetSystem<InterpolationCurvesRenderingSystem>();
    polylineSystem = coordinator.GetSystem<PolylineSystem>();

    gridSystem->Init();
    c0PatchesSystem->Init();
    c2PatchesSystem->Init();
    equidistanceC2System->Init();
    equidistanceC0System->Init();
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


void MillingPathsDesigner::GenerateMainPhase()
{
    const MillingCutter cutter(0.04, MillingCutter::Type::Round);
    MillingMachinePathsBuilder builder;

    builder.AddPosition(millingSettings.initCutterPos);

    GeneratePathsForLeftFin(builder, cutter);
    GeneratePathsForRightFin(builder, cutter);
    GeneratePathsForTorso(builder, cutter);

    builder.AddPosition(millingSettings.initCutterPos);

    auto paths = builder.GetPaths();
    std::vector<Position> pathsPositions;
    pathsPositions.reserve(paths.Size());

    for (const auto& path: paths) {
        pathsPositions.push_back(path.destination);
    }

    polylineSystem->AddPolyline(pathsPositions);

    MillingMachinePathsSystem::CreateGCodeFile(paths, "paths/3.k08");
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


ModelHeightMap MillingPathsDesigner::GenerateHeightMap(const size_t xResolution, const size_t zResolution)
{
    const DepthBuffer depthBuffer(xResolution, zResolution);

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
    ChangeViewportSize(xResolution, zResolution);

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

    ModelHeightMap heightMap(
        xResolution,
        zResolution,
        materialParameters.xLen,
        materialParameters.zLen
    );
    glReadPixels(0, 0, xResolution, zResolution, GL_DEPTH_COMPONENT, GL_FLOAT, heightMap.Data());

    float near = 0.f;
    float far = materialParameters.yLen;
    std::ranges::for_each(heightMap, [near, far](float& d) {
        d = far - ((2.0f * d - 1.0f) * (far - near) + (far + near)) * 0.5f;
    });

    DepthBuffer::UseDefault();
    ChangeViewportSize(oldViewportWidth, oldViewportHeight);

    // glDrawBuffer(GL_BACK);
    // glReadBuffer(GL_BACK);
    glDepthMask(GL_FALSE);

    return heightMap;
}


ModelHeightMap MillingPathsDesigner::GenerateBroadPhaseHeightMap()
{
    constexpr int mapResolution = 300;
    auto heightMap = GenerateHeightMap(mapResolution, mapResolution);

    std::ranges::for_each(heightMap, [this](float& d) {
        d += this->millingSettings.broadPhaseAdditionalThickness;
    });

    return heightMap;
}


float MillingPathsDesigner::MinYCutterPos(
    const ModelHeightMap &heightMap, const MillingCutter &cutter, const float cutterX, const float cutterZ
) {
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




std::vector<alg::Vec2> MillingPathsDesigner::GetPointsVec(const IntersectionCurve &curve)
{
    std::vector<alg::Vec2> result;
    result.reserve(curve.Size());

    for (const auto& p: curve) {
        float u = p.U1();
        float v = p.V1();

        result.emplace_back(u, v);
    }

    return result;
}


void MillingPathsDesigner::InterCurveToFileNormalized(const std::string& fileName, const IntersectionCurve& curve, Entity e)
{
    std::ofstream file(fileName);

    for (const auto& p: curve) {
        float u = p.U1();
        float v = p.V1();

        NormalizeUV(e, u, v);

        file << u << ", " << v << std::endl;
    }
}


std::vector<alg::Vec2> MillingPathsDesigner::ConnectInsidePointToBoundary(
    const std::vector<alg::Vec2> &insidePoints, const std::vector<alg::Vec2> &boundary
) {
    auto const& lastPoint = insidePoints.back();

    const CircularVecWrap circularBoundary(boundary);

    int minDistBoundIdx = 0;
    float minDist = std::numeric_limits<float>::infinity();

    for (int i = 0; i < boundary.size(); ++i) {
        float dist = alg::Distance(circularBoundary[i], lastPoint);

        if (dist < minDist) {
            minDistBoundIdx = i;
            minDist = dist;
        }
    }

    std::vector<alg::Vec2> result;
    result.reserve(insidePoints.size() + boundary.size());

    result.insert(result.end(), insidePoints.begin(), insidePoints.end());

    for (int i = 0; i < boundary.size(); ++i) {
        const int idx = minDistBoundIdx + i;
        result.emplace_back(circularBoundary[idx]);
    }

    return result;
}


Position MillingPathsDesigner::GlobalPosition(const Entity entity, const alg::Vec2 &paramPoint, const MillingCutter &cutter) const
{
    const auto surfaceSystem = GetSurfaceSystem(coordinator, entity);

    auto pos = surfaceSystem->PointOnSurface(entity, paramPoint.X(), paramPoint.Y());

    if (cutter.type == MillingCutter::Type::Round)
        pos.vec.Y() -= cutter.radius;

    return pos;
}


void MillingPathsDesigner::NormalizeUV(const Entity entity, float &u, float &v) const
{
    const auto sys = GetSurfaceSystem(coordinator, entity);

    const float maxU = sys->MaxU(entity);
    const float maxV = sys->MaxV(entity);

    if ((u > maxU || u < 0.0f) && coordinator.HasComponent<WrapU>(entity))
        u -= std::floor(u / maxU) * maxU;

    if ((v > maxV || v < 0.f) && coordinator.HasComponent<WrapV>(entity))
        v -= std::floor(v / maxV) * maxV;
}
