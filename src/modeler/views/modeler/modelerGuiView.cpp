#include <CAD_modeler/views/modeler/modelerGuiView.hpp>

#include <CAD_modeler/utilities/angle.hpp>
#include <CAD_modeler/model/components/unremovable.hpp>
#include <CAD_modeler/controllers/modelerController.hpp>

#include <imgui.h>

#include <stdexcept>
#include <optional>
#include <sstream>


ModelerGuiView::ModelerGuiView(ModelerController& controller, Modeler& model):
    menuBar(controller, model),
    mainMenu(controller, model),
    propertiesView(controller, model)
{
}


void ModelerGuiView::Render()
{
    ImGui::Begin("Modeler");

    menuBar.Render();
    mainMenu.Render();
    
    ImGui::End();

    propertiesView.Render();
}
