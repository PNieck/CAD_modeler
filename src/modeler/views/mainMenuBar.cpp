#include <CAD_modeler/views/mainMenuBar.hpp>

#include <CAD_modeler/controllers/modelerController.hpp>

#include <imgui.h>


MainMenuBar::MainMenuBar(ModelerController& controller, Modeler& model):
    controller(controller), model(model), fileDialog(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir)
{
    fileDialog.SetTitle("Choose file to load");
    fileDialog.SetTypeFilters({ ".json"});
}


void MainMenuBar::Render()
{
    static bool savingScene = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load"))
                fileDialog.Open();

            if (ImGui::MenuItem("Save")) {
                fileDialog.Open();
                savingScene = true;
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

    fileDialog.Display();

    if(fileDialog.HasSelected()) {
        const auto path = fileDialog.GetSelected().string();
        fileDialog.ClearSelected();

        if (savingScene) {
            model.SaveScene(path);
            savingScene = false;
        }
        else
            model.LoadScene(path);
        fileDialog.Close();
    }
}
