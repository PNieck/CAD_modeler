#include "CAD_modeler/model/millingPathsDesigner.hpp"

#include <CAD_modeler/model/managers/loadManager.hpp>

#include <CAD_modeler/model/components/registerComponents.hpp>


MillingPathsDesigner::MillingPathsDesigner(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
    // TODO: remove
    RegisterAllComponents(coordinator);

    GridSystem::RegisterSystem(coordinator);
    PointsSystem::RegisterSystem(coordinator);
    ToriSystem::RegisterSystem(coordinator);
    ToriRenderingSystem::RegisterSystem(coordinator);
    C0PatchesSystem::RegisterSystem(coordinator);
    C0PatchesRenderSystem::RegisterSystem(coordinator);
    C2PatchesSystem::RegisterSystem(coordinator);
    C2PatchesRenderSystem::RegisterSystem(coordinator);
    NameSystem::RegisterSystem(coordinator);

    gridSystem = coordinator.GetSystem<GridSystem>();
    pointsSystem = coordinator.GetSystem<PointsSystem>();
    toriSystem = coordinator.GetSystem<ToriSystem>();
    toriRenderingSystem = coordinator.GetSystem<ToriRenderingSystem>();
    c0PatchesSystem = coordinator.GetSystem<C0PatchesSystem>();
    c0PatchesRenderSystem = coordinator.GetSystem<C0PatchesRenderSystem>();
    c2PatchesSystem = coordinator.GetSystem<C2PatchesSystem>();
    c2PatchesRenderSystem = coordinator.GetSystem<C2PatchesRenderSystem>();
    nameSystem = coordinator.GetSystem<NameSystem>();

    gridSystem->Init();
    c0PatchesSystem->Init();
    c2PatchesSystem->Init();
}


void MillingPathsDesigner::LoadModel(const std::string &filePath)
{
    LoadManager loadManager;
    loadManager.Load(filePath, coordinator, LoadManager::AllSurfaces);
}

void MillingPathsDesigner::Update()
{
    c0PatchesSystem->Update();
    c2PatchesSystem->Update();
}


void MillingPathsDesigner::RenderSystemsObjects(
    const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx, const float nearPlane, const float farPlane) const
{
    const alg::Mat4x4 cameraMtx = persMtx * viewMtx;

    toriRenderingSystem->Render(cameraMtx);
    c0PatchesRenderSystem->Render(cameraMtx);
    c2PatchesRenderSystem->Render(cameraMtx);

    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
}
