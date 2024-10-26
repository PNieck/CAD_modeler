#include "CAD_modeler/model/millingMachineSim.hpp"


MillingMachineSim::MillingMachineSim(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
    GridSystem::RegisterSystem(coordinator);
    MillingMachineSystem::RegisterSystem(coordinator);

    gridSystem = coordinator.GetSystem<GridSystem>();
    millingMachineSystem = coordinator.GetSystem<MillingMachineSystem>();

    ShaderRepository& shadersRepo = ShaderRepository::GetInstance();

    gridSystem->Init(&shadersRepo);

    millingMachineSystem->CreateMaterial(1000, 1000);
}


void MillingMachineSim::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx,
    const float nearPlane, const float farPlane) const
{
    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
    millingMachineSystem->Render(viewMtx, persMtx);
}
