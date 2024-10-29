#pragma once

#include "../model/millingMachineSim.hpp"

#include <string>

// ImGui header must be included before imfilebrowser one
#include <imgui.h>
#include <imfilebrowser.h>


class MillingSimController;


class MillingSimulatorView {
public:
    MillingSimulatorView(MillingSimController& controller, MillingMachineSim& model);

    void RenderGui();

private:
    MillingSimController& controller;
    MillingMachineSim& model;

    std::string filePath;

    ImGui::FileBrowser fileBrowser;

    void RenderFileSelection();
    void RenderMaterialOptions() const;
    void RenderSimulationOptions() const;
    void RenderCutterInformation() const;
};
