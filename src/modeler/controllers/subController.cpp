#include <CAD_modeler/controllers/subController.hpp>


MouseState SubController::mouseState = {};
KeyboardState SubController::keyState = {};


void SubController::MouseMove(const int x, const int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(Middle)) {
        auto offset = mouseState.TranslationGet();

        if (!keyState.ShiftPressed()) {
            model.cameraManager.RotateCamera(
                offset.X() * ROTATION_COEF,
                offset.Y() * ROTATION_COEF
            );
        }
        else {
            model.cameraManager.MoveCameraWithTarget(offset.X() * 0.02f, offset.Y() * 0.02f);
        }
    }
}


void SubController::ScrollMoved(const int offset)
{
    float val = static_cast<float>(offset) * SCROLL_COEF;

    if (val < 0.0f) {
        val = -1.0f / val;
    }

    const float dist = model.cameraManager.GetDistanceFromTarget();
    model.cameraManager.SetDistanceFromTarget(dist*val);
}
