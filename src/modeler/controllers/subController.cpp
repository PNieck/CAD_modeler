#include <CAD_modeler/controllers/subController.hpp>


MouseState SubController::mouseState = {};


void SubController::MouseMove(const int x, const int y, Model &model)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();
        model.cameraManager.RotateCamera(
            offset.Y() * ROTATION_COEF,
            offset.X() * ROTATION_COEF
        );
    }
}


void SubController::ScrollMoved(const int offset, Model &model)
{
    float val = offset * SCROLL_COEF;

    if (val < 0.0f) {
        val = -1.0f / val;
    }

    const float dist = model.cameraManager.GetDistanceFromTarget();
    model.cameraManager.SetDistanceFromTarget(dist*val);
}
