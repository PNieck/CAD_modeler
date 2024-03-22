#include <CAD_modeler/controllers/mainController.hpp>


MainController::MainController(GLFWwindow* window, int window_width, int window_height):
    model(window_width, window_height), glController(model), guiController(model), guiView(window)
{
}


void MainController::MouseMoved(int x, int y)
{
    if (guiController.WantCaptureMouse(x, y))
        return;

    glController.MouseMove(x, y);
}


void MainController::Render()
{
    model.RenderFrame();
    guiView.RenderGui(guiController, model);
}
