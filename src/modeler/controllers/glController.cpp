#include <CAD_modeler/controllers/glController.hpp>

#include <CAD_modeler/controllers/mainController.hpp>
#include <CAD_modeler/utilities/setIntersection.hpp>

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
    Entity point = model.Add3DPointFromViewport(x, y);

    // Add control points to curves if selected
    auto const& curves = model.GetAllCurves();
    auto const& selected = model.GetAllSelectedEntities();

    auto selectedCurves = intersect(curves, selected);

    for (auto curve: selectedCurves) {
        model.AddControlPointToCurve(curve, point);
    }
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
        model.cameraManager.RotateCamera(
            offset.Y() * ROTATION_COEFF,
            offset.X() * ROTATION_COEFF
        );
    }
}


std::tuple<float,float> GlController::MouseToViewportCoordinates() const
{
    int windowWidth, windowHeight;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int halfWidth = windowWidth / 2;
    int halfHeight = windowHeight / 2;

    auto mousePos = mouseState.PositionGet();
    float x = (float)(mousePos.X() - halfWidth) / (float)halfWidth;
    float y = -(float)(mousePos.Y() - halfHeight) / (float)halfHeight;

    return std::make_tuple(x, y);
}


void GlController::ScrollMoved(int offset)
{
    float val = offset * SCROLL_COEFF;

    if (val < 0.0f) {
        val = (-1.0f) / val;
    }

    float dist = model.cameraManager.GetDistanceFromTarget();
    model.cameraManager.SetDistanceFromTarget(dist*val);
}
