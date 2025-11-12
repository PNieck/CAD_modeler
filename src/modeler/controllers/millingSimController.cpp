#include <CAD_modeler/controllers/millingSimController.hpp>


MillingSimController::MillingSimController(MillingMachineSim &millingSim):
    SubController(millingSim), simulator(millingSim), view(simulator)
{
}


void MillingSimController::Render()
{
    simulator.RenderFrame();
    view.RenderGui();
}
