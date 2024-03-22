#include <CAD_modeler/controllers/guiController.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


GuiController::GuiController(Model & model):
    model(model)
{
}


bool GuiController::WantCaptureMouse(int xPos, int yPos)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(xPos, yPos);

    return io.WantCaptureMouse;
}


void GuiController::TorusSmallRadiusChanged(float newRadius)
{
    Torus& torus = model.TorusGet();
    torus.SmallRadiusSet(newRadius);
}


void GuiController::TorusBigRadiusChanged(float newRadius)
{
    model.ChangeTorusBigRadius(newRadius);
}
