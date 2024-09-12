#pragma once

#include "../controllers/guiController.hpp"
#include "../model.hpp"

#include <imgui.h>
#include <imfilebrowser.h>


class MainMenuBar {
public:
    MainMenuBar(GuiController& controller, const Model& model);

    void Render();

private:
    GuiController& controller;
    const Model& model;

    ImGui::FileBrowser fileDialog;
};
