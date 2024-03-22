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
        Torus& torus = model.TorusGet();

        torus.rotate_x(offset.y * ROTATION_COEFF);
        torus.rotate_y(offset.x * ROTATION_COEFF);
    }

    if (mouseState.IsButtonClicked(MouseButton::Middle)) {
        auto offset = mouseState.TranslationGet();
        Torus& torus = model.TorusGet();

        torus.MoveX(offset.x * TRANSLATION_COEFF);
        torus.MoveY(-offset.y * TRANSLATION_COEFF);
    }
}


void GlController::ScrollMoved(int offset)
{
    Torus& torus = model.TorusGet();
    float new_scale = torus.ScaleGet() + offset * SCROLL_COEFF;

    if (new_scale > 0)
        torus.ScaleSet(new_scale);
}
