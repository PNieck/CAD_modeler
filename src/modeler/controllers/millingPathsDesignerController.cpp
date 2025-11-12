#include <CAD_modeler/controllers/millingsPathsDesignerController.hpp>


MillingsPathsDesignerController::MillingsPathsDesignerController(MillingPathsDesigner &model):
    SubController(model), model(model), view(model)
{
}


void MillingsPathsDesignerController::Render()
{
    model.RenderFrame();
    view.RenderGui();
}
