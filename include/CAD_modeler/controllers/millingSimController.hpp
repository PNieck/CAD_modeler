#pragma once

#include "../model/millingMachineSim.hpp"
#include "../views/millingSimulatorView.hpp"
#include "subController.hpp"


class MillingSimController: public SubController {
public:
    explicit MillingSimController(MillingMachineSim& millingSim);

    void Update(const double dt) override
        { simulator.Update(dt); }

    void Render() override;

private:
    MillingMachineSim& simulator;
    MillingSimulatorView view;
};
