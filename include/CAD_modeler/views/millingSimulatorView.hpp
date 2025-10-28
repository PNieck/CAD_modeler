#pragma once

#include "../model/millingMachineSim.hpp"

#include <string>


class MillingSimulatorView {
public:
    explicit MillingSimulatorView(MillingMachineSim& model);

    void RenderGui();

private:
    MillingMachineSim& model;

    std::string filePath;

    void RenderFileSelection();
    void RenderMaterialOptions() const;
    void RenderSimulationOptions() const;
    void RenderCutterInformation() const;
    void RenderWarnings() const;
};
