#include <CAD_modeler/views/mainMenuBar.hpp>

#include <imgui.h>


void MainMenuBar::Render() const
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
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
}
