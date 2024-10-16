#include <CAD_modeler/views/mainMenuBar.hpp>

#include <imgui.h>


MainMenuBar::MainMenuBar(GuiController &controller, const Modeler &model):
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
            auto cameraType = model.cameraManager.GetCurrentCameraType();

            if (ImGui::MenuItem("Normal", NULL, cameraType == CameraManager::CameraType::Perspective)) {
                controller.SetCameraType(GuiController::CameraType::Perspective);

                if (controller.GetAppState() == AppState::AnaglyphsSettings)
                    controller.SetAppState(AppState::Default);
            }

            if (ImGui::MenuItem("Anaglyphs", NULL, cameraType == CameraManager::CameraType::Anaglyphs)) {
                controller.SetCameraType(GuiController::CameraType::Anaglyphs);
                controller.SetAppState(AppState::AnaglyphsSettings);
            }

            ImGui::EndMenu();
        }

#       ifndef NDEBUG
            static bool renderImGuiDemo = false;
            if (ImGui::BeginMenu("Debug")) {
                if (ImGui::MenuItem("Show Imgui demo window", NULL, renderImGuiDemo))
                    renderImGuiDemo = !renderImGuiDemo;

                ImGui::EndMenu();
            }

            if (renderImGuiDemo)
                ImGui::ShowDemoWindow();
                
#       endif  // NDEBUG

        ImGui::EndMainMenuBar();
    }

    fileDialog.Display();

    if(fileDialog.HasSelected())
    {
        auto path = fileDialog.GetSelected().string();
        fileDialog.ClearSelected();

        if (savingScene) {
            controller.SaveScene(path);
            savingScene = false;
        }
        else
            controller.LoadScene(path);
        fileDialog.Close();
    }
}
