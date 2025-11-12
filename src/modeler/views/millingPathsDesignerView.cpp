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

    RenderFileSelection();

    if (ImGui::Button("Broad phase"))
        model.GenerateBroadPhase();

    ImGui::End();
}

void MillingPathsDesignerView::RenderFileSelection()
{
    ImGui::SeparatorText("File");

    if (filePath.empty())
        ImGui::Text("No file selected");
    else
        ImGui::Text("Loaded file: %s", filePath.c_str());

    if (ImGui::Button("Load")) {
        IGFD::FileDialogConfig config;
        config.path = ".";

        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseModelFileDlgKey",
            "Choose Model",
            ".json",
            config
        );
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseModelFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            model.LoadModel(filePath);
        }

        ImGuiFileDialog::Instance()->Close();
    }
}
