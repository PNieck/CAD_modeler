#include <CAD_modeler/controllers/subController.hpp>

#include <CAD_modeler/controllers/mainController.hpp>


AppState SubController::GetAppState() const
{
    return mainController.GetAppState();
}


void SubController::SetAppState(AppState newState) const
{
    mainController.SetAppState(newState);
}
