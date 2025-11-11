#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>
#include <CAD_modeler/model/components/drawStd.hpp>
#include <CAD_modeler/model/millingPathsDesigner/depthBuffer.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/c0PatchesTrianglesRenderSystem.hpp>
#include <CAD_modeler/model/systems/c2PatchesTrianglesRenderingSystem.hpp>
#include <CAD_modeler/model/systems/controlPointsRegistrySystem.hpp>

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
    auto broadPhaseHeightMap = GenerateBroadPhaseHeightMap();


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


FlatVec2D<float> MillingPathsDesigner::GenerateBroadPhaseHeightMap()
{
    constexpr int depthBufferResolution = 2000;
    DepthBuffer depthBuffer(depthBufferResolution, depthBufferResolution);

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

    FlatVec2D<float> depthData(depthBufferResolution, depthBufferResolution);
    glReadPixels(0, 0, depthBufferResolution, depthBufferResolution, GL_DEPTH_COMPONENT, GL_FLOAT, depthData.Data());

    float near = 0.f;
    float far = materialParameters.yLen;
    std::ranges::for_each(depthData, [near, far](float& d) {
        d = ((2.0f * d - 1.0f) * (far - near) + (far + near)) * 0.5f;
    });

    std::ranges::for_each(depthData, [this](float& d) {
        d += this->millingSettings.broadPhaseAdditionalThickness;
    });

    ToPGM(depthData, "depth.pgm");

    DepthBuffer::UseDefault();
    ChangeViewportSize(oldViewportWidth, oldViewportHeight);

    // glDrawBuffer(GL_BACK);
    // glReadBuffer(GL_BACK);
    glDepthMask(GL_FALSE);

    return depthData;
}
