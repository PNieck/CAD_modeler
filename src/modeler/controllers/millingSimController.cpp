#include <CAD_modeler/controllers/millingSimController.hpp>


MillingSimController::MillingSimController(MillingMachineSim &millingSim):
    simulator(millingSim)
{
}

void MillingSimController::Render() const
{
    simulator.RenderFrame();
}
