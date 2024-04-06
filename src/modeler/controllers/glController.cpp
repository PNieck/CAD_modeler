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

    if (button == MouseButton::Left) {
        switch (GetAppState())
        {
        case AppState::Default:
            MoveCursor();
            break;

        case AppState::Adding3dPoints:
            Add3DPoint();
            break;
        
        default:
            break;
        }
    }
    else if (button == MouseButton::Right) {
        TryToSelectObject();
    }
}


void GlController::MoveCursor() const
{
    auto [x, y] = MouseToViewportCoordinates();
    model.SetCursorPositionFromViewport(x, y);
}


void GlController::Add3DPoint() const
{
    auto [x, y] = MouseToViewportCoordinates();
    model.Add3DPointFromViewport(x, y);
}


void GlController::TryToSelectObject() const
{
    auto [x, y] = MouseToViewportCoordinates();
    model.TryToSelectFromViewport(x, y);
}


void GlController::MouseMove(int x, int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();
        model.RotateCamera(offset.y * 0.02f, offset.x * 0.02f);
    }
}


std::tuple<float,float> GlController::MouseToViewportCoordinates() const
{
    int windowWidth, windowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int halfWidth = windowWidth / 2;
    int halfHeight = windowHeight / 2;

    auto mousePos = mouseState.PositionGet();
    float x = (float)(mousePos.x - halfWidth) / (float)halfWidth;
    float y = -(float)(mousePos.y - halfHeight) / (float)halfHeight;

    return std::make_tuple(x, y);
}


void GlController::ScrollMoved(int offset)
{
    float val = offset * SCROLL_COEFF;

    if (val < 0.0f) {
        val = (-1.0f) / val;
    }

    model.MultiplyCameraDistanceFromTarget(val);
}
