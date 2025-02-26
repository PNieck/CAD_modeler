#pragma once

#include "../model/millingMachineSim.hpp"

#include <string>


class MillingSimController;


class MillingSimulatorView {
public:
    MillingSimulatorView(MillingSimController& controller, MillingMachineSim& model);

    void RenderGui();

private:
    MillingSimController& controller;
    MillingMachineSim& model;

    std::string filePath;

    void RenderFileSelection();
    void RenderMaterialOptions() const;
    void RenderSimulationOptions() const;
    void RenderCutterInformation() const;
    void RenderWarnings() const;
};
