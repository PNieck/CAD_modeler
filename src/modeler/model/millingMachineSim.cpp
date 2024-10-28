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

    millingMachineSystem->CreateMaterial(1000, 1000);

    auto paths = MillingMachinePathsSystem::ParseGCode("C:\\Users\\piotr\\source\\repos\\CAD_modeler\\tmp\\1.k16");
    auto cutter = MillingMachinePathsSystem::ParseCutter("C:\\Users\\piotr\\source\\repos\\CAD_modeler\\tmp\\1.k16");
    cutter.radius *= 4.f;
    millingMachineSystem->AddPaths(std::move(paths), cutter);

    millingMachineSystem->StartMachine();

    //glEnable(GL_DEPTH_TEST);
}


void MillingMachineSim::Update(const double dt) const
{
    millingMachineSystem->Update(dt);
}


void MillingMachineSim::RenderSystemsObjects(const alg::Mat4x4 &viewMtx, const alg::Mat4x4 &persMtx,
                                             const float nearPlane, const float farPlane) const
{
    gridSystem->Render(viewMtx, persMtx, nearPlane, farPlane);
    millingMachineSystem->Render(viewMtx, persMtx);
}
