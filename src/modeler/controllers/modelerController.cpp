#include <CAD_modeler/controllers/modelerController.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>


ModelerController::ModelerController(Modeler &modeler):
    SubController(modeler),
    modeler(modeler),
    guiView(*this, modeler),
    modelerState(ModelerState::Default)
{
}

void ModelerController::Render()
{
    modeler.RenderFrame();
    guiView.Render();
}


void ModelerController::MouseClick(const MouseButton button)
{
    SubController::MouseClick(button);

    if (button == Left) {
        switch (GetModelerState())
        {
            case ModelerState::Default:
                Move3DCursor();
            break;

            case ModelerState::Adding3dPoints:
                Add3DPoint();
            break;

            default:
                break;
        }
    }
    else if (button == Right) {
        TryToSelectObject();
    }
}


void ModelerController::MouseMove(const int x, const int y)
{
    SubController::MouseMove(x, y);

    if (modeler.selectingEntities) {
        auto [viewportX, viewportY] = MouseToViewportCoordinates();

        modeler.selectionCircleX = viewportX;
        modeler.selectionCircleY = viewportY;

        if (mouseState.IsButtonClicked(MouseButton::Left)) {
            modeler.SelectMultipleFromViewport(
                modeler.selectionCircleX,
                modeler.selectionCircleY,
                modeler.selectionCircleRadius
            );
        }
    }
}


void ModelerController::KeyboardKeyPressed(const KeyboardKey key)
{
    SubController::KeyboardKeyPressed(key);

    if (key == KeyboardKey::c)
        modeler.selectingEntities = !modeler.selectingEntities;


}


void ModelerController::Move3DCursor()
{
    auto [x, y] = MouseToViewportCoordinates();
    modeler.SetCursorPositionFromViewport(x, y);
}


void ModelerController::Add3DPoint()
{
    auto [x, y] = MouseToViewportCoordinates();
    const Entity point = modeler.Add3DPointFromViewport(x, y);

    // Add control points to curves if selected
    auto const& curves = modeler.GetAllCurves();
    auto const& selected = modeler.GetAllSelectedEntities();

    const auto selectedCurves = intersect(curves, selected);

    for (auto curve: selectedCurves) {
        if (modeler.GetAllC0Curves().contains(curve))
            modeler.AddControlPointToC0Curve(curve, point);
        else if (modeler.GetAllC2Curves().contains(curve))
            modeler.AddControlPointToC2Curve(curve, point);
        else if (modeler.GetAllInterpolationCurves().contains(curve))
            modeler.AddControlPointToInterpolationCurve(curve, point);
        else
            throw std::runtime_error("Unknown curve type");
    }
}


void ModelerController::TryToSelectObject()
{
    auto [x, y] = MouseToViewportCoordinates();
    modeler.TryToSelectFromViewport(x, y);
}


std::tuple<float, float> ModelerController::MouseToViewportCoordinates() const
{
    auto [windowWidth, windowHeight] = modeler.GetViewportSize();

    const int halfWidth = windowWidth / 2;
    const int halfHeight = windowHeight / 2;

    auto mousePos = mouseState.PositionGet();
    float x = static_cast<float>(mousePos.X() - halfWidth) / static_cast<float>(halfWidth);
    float y = -static_cast<float>(mousePos.Y() - halfHeight) / static_cast<float>(halfHeight);

    return std::make_tuple(x, y);
}
