#include <CAD_modeler/controllers/millingSimController.hpp>


MillingSimController::MillingSimController(MillingMachineSim &millingSim):
    simulator(millingSim), view(*this, simulator)
{
}

void MillingSimController::Render()
{
    simulator.RenderFrame();
    view.RenderGui();
}
