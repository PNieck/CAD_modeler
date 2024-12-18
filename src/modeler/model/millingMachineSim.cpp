#include "CAD_modeler/model/millingMachineSim.hpp"

#include <CAD_modeler/model/systems/millingMachinePathsSystem.hpp>


MillingMachineSim::MillingMachineSim(const int viewportWidth, const int viewportHeight):
    Model(viewportWidth, viewportHeight)
{
    GridSystem::RegisterSystem(coordinator);
    MillingMachineSystem::RegisterSystem(coordinator);

    gridSystem = coordinator.GetSystem<GridSystem>();
    millingMachineSystem = coordinator.GetSystem<MillingMachineSystem>();

    ShaderRepository& shadersRepo = ShaderRepository::GetInstance();

    gridSystem->Init(&shadersRepo);

    millingMachineSystem->Init(1000, 1000);
}


void MillingMachineSim::Update(const double dt) const
{
    millingMachineSystem->Update(dt);
}


void MillingMachineSim::AddMillingPath(const std::string &filePath) const
{
    auto paths = MillingMachinePathsSystem::ParseGCode(filePath);
    const auto cutter = MillingMachinePathsSystem::ParseCutter(filePath);

    millingMachineSystem->AddPaths(std::move(paths), cutter);
}


void MillingMachineSim::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx,
                                             const float nearPlane, const float farPlane) const
{
    millingMachineSystem->Render(viewMtx, persMtx, cameraManager.GetCameraPosition().vec);
    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
}
