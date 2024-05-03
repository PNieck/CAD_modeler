#include <CAD_modeler/controllers/guiController.hpp>

#include <stdexcept>


GuiController::GuiController(Model & model, MainController & controller):
    SubController(model, controller)
{
}


Entity GuiController::AddCurve(const std::vector<Entity>& entities, CurveType curveType) const
{
    switch (curveType)
    {
    case CurveType::C0:
        return model.AddC0Curve(entities);
    
    case CurveType::C2:
        return model.AddC2Curve(entities);

    default:
        throw std::runtime_error("Unknown curve type");
    }
}
