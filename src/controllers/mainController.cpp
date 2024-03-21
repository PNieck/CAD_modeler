#include <CAD_modeler/controllers/mainController.hpp>


MainController::MainController(int window_width, int window_height):
    model(window_width, window_height)
{
}


void MainController::Render()
{
    model.RenderFrame();
}
