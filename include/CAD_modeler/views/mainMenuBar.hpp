#pragma once

#include "../model/modeler.hpp"

// ImGui header must be included before imfilebrowser one
#include <imgui.h>
#include <imfilebrowser.h>


class ModelerController;


class MainMenuBar {
public:
    MainMenuBar(ModelerController& controller, Modeler& model);

    void Render();

private:
    ModelerController& controller;
    Modeler& model;

    ImGui::FileBrowser fileDialog;
};
