#include <CAD_modeler/views/mainMenuBar.hpp>

#include <imgui.h>


MainMenuBar::MainMenuBar(GuiController &controller, const Model &model):
    controller(controller), model(model)
{
    fileDialog.SetTitle("Choose file to load");
    fileDialog.SetTypeFilters({ ".json"});
}


void MainMenuBar::Render()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load"))
                fileDialog.Open();

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

        ImGui::EndMainMenuBar();
    }

    if (true) {
        fileDialog.Display();

        if(fileDialog.HasSelected())
        {
            auto path = fileDialog.GetSelected().string();
            fileDialog.ClearSelected();

            controller.LoadScene(path);
            fileDialog.Close();
        }
    }
}
