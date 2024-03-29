#include <CAD_modeler/controllers/glController.hpp>

#include <stdexcept>


GlController::GlController(Model& model):
    model(model)
{
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
