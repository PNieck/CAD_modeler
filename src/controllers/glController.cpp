#include <CAD_modeler/controllers/glController.hpp>

#include <CAD_modeler/controllers/mainController.hpp>

#include <stdexcept>


GlController::GlController(Model& model, GLFWwindow* window, MainController& controller):
    window(window), SubController(model, controller)
{
}


void GlController::MouseClick(MouseButton button)
{
    mouseState.ButtonClicked(button);

    if (button == MouseButton::Left && GetAppState() == AppState::Default) {
        MoveCursor();
    }
}


void GlController::MoveCursor() const
{
    int windowWidth, windowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int halfWidth = windowWidth / 2;
    int halfHeight = windowHeight / 2;

    auto mousePos = mouseState.PositionGet();
    float x = (float)(mousePos.x - halfWidth) / (float)halfWidth;
    float y = (float)(mousePos.y - halfHeight) / (float)halfHeight;
    model.SetCursorPositionFromWindowPoint(x, -y);
}


void GlController::MouseMove(int x, int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();
        model.RotateCamera(offset.y * 0.02f, offset.x * 0.02f);
    }
}


void GlController::ScrollMoved(int offset)
{
    float val = offset * SCROLL_COEFF;

    if (val < 0.0f) {
        val = (-1.0f) / val;
    }

    model.MultiplyCameraDistanceFromTarget(val);
}
