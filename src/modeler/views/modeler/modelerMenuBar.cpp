#include <CAD_modeler/views/modeler/modelerMenuBar.hpp>

#include <CAD_modeler/controllers/modelerController.hpp>

#include <imgui.h>
#include <ImGuiFileDialog.h>


ModelerMenuBar::ModelerMenuBar(ModelerController& controller, Modeler& model):
    controller(controller), model(model)
{
}


void ModelerMenuBar::Render() const
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("LoadFileDlgKey", "Choose file to load", ".json", config);
            }

            if (ImGui::MenuItem("Save")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("SaveFileDlgKey", "Choose or create file to save", ".json", config);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera")) {
            const auto cameraType = model.cameraManager.GetCurrentCameraType();

            if (ImGui::MenuItem("Normal", nullptr, cameraType == CameraManager::CameraType::Perspective)) {
                model.cameraManager.SetCameraType(CameraManager::CameraType::Perspective);

                if (controller.GetModelerState() == ModelerState::AnaglyphsSettings)
                    controller.SetModelerState(ModelerState::Default);
            }

            if (ImGui::MenuItem("Anaglyphs", nullptr, cameraType == CameraManager::CameraType::Anaglyphs)) {
                model.cameraManager.SetCameraType(CameraManager::CameraType::Anaglyphs);
                controller.SetModelerState(ModelerState::AnaglyphsSettings);
            }

            if (ImGui::MenuItem("Center camera")) {
                auto params = model.cameraManager.GetBaseParams();
                params.target = Position(0.f);
                model.cameraManager.SetBaseParams(params);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Clear")) {
                model.ClearScene();
            }

            ImGui::EndMenu();
        }

#       ifndef NDEBUG // is true only during debug compilation

            static bool renderImGuiDemo = false;
            if (ImGui::BeginMenu("Debug")) {
                if (ImGui::MenuItem("Show ImGui demo window", nullptr, renderImGuiDemo))
                    renderImGuiDemo = !renderImGuiDemo;

                ImGui::EndMenu();
            }

            if (renderImGuiDemo)
                ImGui::ShowDemoWindow();
                
#       endif  // NDEBUG

        ImGui::EndMainMenuBar();
    }

    if (ImGuiFileDialog::Instance()->Display("LoadFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            model.LoadScene(ImGuiFileDialog::Instance()->GetFilePathName());
        }

        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            model.SaveScene(ImGuiFileDialog::Instance()->GetFilePathName());
        }

        ImGuiFileDialog::Instance()->Close();
    }
}
