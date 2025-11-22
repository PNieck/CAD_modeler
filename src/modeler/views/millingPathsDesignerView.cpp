#include <CAD_modeler/views/millingPathsDesignerView.hpp>

#include <imgui.h>
#include <ImGuiFileDialog.h>


MillingPathsDesignerView::MillingPathsDesignerView(MillingPathsDesigner &model):
    model(model)
{
}


void MillingPathsDesignerView::RenderGui()
{
    ImGui::Begin("Milling paths designer");

    if (ImGui::Button("Broad phase"))
        model.GenerateBroadPhase();

    if (ImGui::Button("Base phase"))
        model.GenerateBasePhase();

    if (ImGui::Button("Main phase"))
        model.GenerateMainPhase();

    ImGui::End();
}
