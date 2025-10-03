#pragma once

#include "../model/millingMachineSim.hpp"
#include "../views/millingSimulatorView.hpp"
#include "subController.hpp"


class MillingSimController: public SubController {
public:
    explicit MillingSimController(MillingMachineSim& millingSim);

    void Update(const double dt) const
        { simulator.Update(dt); }

    void WindowSizeChanged(const int width, const int height) const
        { SubController::WindowSizeChanged(width, height, simulator); }

    void MouseClick(const MouseButton button) const
        { SubController::MouseClick(button); }

    void MouseRelease(const MouseButton button) const
        { SubController::MouseRelease(button); }

    void MouseMove(const int x, const int y) const
        { SubController::MouseMove(x, y, simulator); }

    void ScrollMoved(const int offset) const
        { SubController::ScrollMoved(offset, simulator); }

    void KeyboardKeyPressed(const KeyboardKey key) const
        { SubController::KeyboardKeyPressed(key); }

    void KeyboardKeyReleased(const KeyboardKey key)
        { SubController::KeyboardKeyReleased(key); }

    void Render();

private:
    MillingMachineSim& simulator;
    MillingSimulatorView view;
};
