#include <CAD_modeler/controllers/glController.hpp>

#include <stdexcept>


GlController::GlController(Model& model):
    model(model)
{
}


void GlController::MouseMove(int x, int y)
{
    mouseState.Moved(x, y);

    if (mouseState.IsButtonClicked(MouseButton::Left)) {
        auto offset = mouseState.TranslationGet();

        model.RotateTorusX(offset.y * ROTATION_COEFF);
        model.RotateTorusY(offset.x * ROTATION_COEFF);
    }

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();

        model.MoveTorusX(offset.x * TRANSLATION_COEFF);
        model.MoveTorusY(-offset.y * TRANSLATION_COEFF);
    }
}


void GlController::ScrollMoved(int offset)
{
    float new_scale = model.TorusScaleGet() + offset * SCROLL_COEFF;

    if (new_scale > 0)
        model.TorusScaleSet(new_scale);
}
