#include <CAD_modeler/controllers/subController.hpp>


MouseState SubController::mouseState = {};
bool SubController::shiftClicked = false;


void SubController::MouseMove(const int x, const int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(Middle)) {
        auto offset = mouseState.TranslationGet();

        if (!shiftClicked) {
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


void SubController::KeyboardKeyPressed(const KeyboardKey key) {
    if (key == KeyboardKey::left_shift || key == KeyboardKey::right_shift)
        shiftClicked = true;
}


void SubController::KeyboardKeyReleased(const KeyboardKey key) {
    if (key == KeyboardKey::left_shift || key == KeyboardKey::right_shift)
        shiftClicked = false;
}

