#include <CAD_modeler/controllers/mainController.hpp>


MainController::MainController(GLFWwindow* window, int window_width, int window_height):
    model(window_width, window_height),
    glController(model, window, *this),
    guiController(model, *this),
    guiView(window, guiController, model),
    appState(AppState::Default)
{
}


void MainController::MouseMoved(int x, int y)
{
    if (guiController.WantCaptureMouse())
        return;

    glController.MouseMove(x, y);
}


void MainController::MouseClicked(MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    glController.MouseClick(button);
}


void MainController::MouseReleased(MouseButton button)
{
    if (guiController.WantCaptureMouse())
        return;

    glController.MouseRelease(button);
}


void MainController::Render()
{
    // FIXME: use only views
    model.RenderFrame();
    guiView.RenderGui();
}


void MainController::ScrollMoved(int offset)
{
    if (guiController.WantCaptureMouse())
        return;

    glController.ScrollMoved(offset);
}


void MainController::KeyboardKeyPressed(KeyboardKey key)
{
    // if (guiController.WantCaptureKeyboard())
    //     return;
    
    glController.KeyboardKeyPressed(key);
}
