#pragma once

#include "../controllers/guiController.hpp"
#include "../modeler.hpp"

#include <imgui.h>
#include <imfilebrowser.h>


class MainMenuBar {
public:
    MainMenuBar(GuiController& controller, const Modeler& model);

    void Render();

private:
    GuiController& controller;
    const Modeler& model;

    ImGui::FileBrowser fileDialog;
};
