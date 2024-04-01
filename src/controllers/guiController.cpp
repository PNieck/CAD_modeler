#include <CAD_modeler/controllers/guiController.hpp>

#include <stdexcept>


GuiController::GuiController(Model & model):
    model(model)
{
}


void GuiController::TorusSmallRadiusChanged(float newRadius)
{
    // Torus& torus = model.TorusGet();
    // torus.SmallRadiusSet(newRadius);
}


void GuiController::TorusBigRadiusChanged(float newRadius)
{
    //model.ChangeTorusBigRadius(newRadius);
}
