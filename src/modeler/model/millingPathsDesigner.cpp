#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>

#include <CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>
#include <CAD_modeler/model/millingPathsDesigner/millingMachinePathsBuilder.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesTrianglesRenderSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesTrianglesRenderingSystem.hpp>
#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>
#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>

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

    gridSystem = coordinator.GetSystem<GridSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c0PatchesRenderSystem = coordinator.GetSystem<C0PatchesRenderSystem>();
    c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    c2PatchesRenderSystem = coordinator.GetSystem<C2PatchesRenderSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();
    const auto selectionSys = coordinator.GetSystem<SelectionSystem>();

    gridSystem->Init();
    c0PatchesSystem->Init();
    c2PatchesSystem->Init();
    selectionSys->Init();

    base = c0PatchesSystem->CreatePlane(
    alg::Vec3(-materialParameters.xLen/2.f, millingSettings.baseThickness, -materialParameters.zLen/2.f),
    alg::Vec3::UnitY(),
    materialParameters.xLen, materialParameters.zLen
    );
    coordinator.AddComponent<DrawStd>(base, DrawStd());

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

    const float stepLenInZDir = cutter.radius * 1.8f;

    const int stepsInZDir = static_cast<int>(std::ceil((maxZCutterPos - minZCutterPos) / stepLenInZDir));
    const int stepsInXDir = static_cast<int>(std::ceil((maxXCutterPos - minXCutterPos) / heightMap.PixelXLen()));

    for (int stepZ=0; stepZ < stepsInZDir; stepZ++) {
        float actCutterMiddleZ = minZCutterPos + static_cast<float>(stepZ) * stepLenInZDir;

        for (int stepX=0; stepX < stepsInXDir; stepX++) {
            float actCutterMiddleX;

            if (stepZ % 2 == 0)
                actCutterMiddleX = minXCutterPos + static_cast<float>(stepX) * heightMap.PixelXLen();
            else
                actCutterMiddleX = maxXCutterPos - static_cast<float>(stepX) * heightMap.PixelXLen();

            float minCutterY = MinYCutterPos(heightMap, cutter, actCutterMiddleX, actCutterMiddleZ);

            Position newCutterPos(actCutterMiddleX, minCutterY, actCutterMiddleZ);
            builder.AddPosition(newCutterPos);
        }

        if (stepZ < stepsInZDir - 1)
            builder.AddPositionFromOffset(alg::Vec3::UnitZ() * stepLenInZDir);
    }

    heightMap.defaultHeight = materialParameters.yLen - toMill;

    auto prevPos = builder.GetLastPosition();
    prevPos.SetY(heightMap.defaultHeight);
    builder.AddPosition(prevPos);



    MillingMachinePathsSystem::CreateGCodeFile(builder.GetPaths(), "1.k16");
}


void MillingPathsDesigner::RenderSystemsObjects(
    const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, const float nearPlane, const float farPlane) const
{
    const alg::Mat4x4 cameraMtx = persMtx * viewMtx;

    c0PatchesRenderSystem->Render(cameraMtx);
    c2PatchesRenderSystem->Render(cameraMtx);

    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
}


// TODO: remove
#include <CAD_modeler/utilities/toPGM.hpp>


BroadPhaseHeightMap MillingPathsDesigner::GenerateBroadPhaseHeightMap()
{
    constexpr int depthBufferResolution = 2000;
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

    const alg::Mat4x4 viewMatrix = alg::LookAt(camPos, alg::Vec3::UnitY(), alg::Vec3::UnitZ());
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
    //FlatVec2D<float> depthData(depthBufferResolution, depthBufferResolution);
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
