#pragma once

#include "../model/millingMachineSim.hpp"
#include "subController.hpp"


class MillingSimController: public SubController {
public:
    explicit MillingSimController(MillingMachineSim& millingSim);

    inline void WindowSizeChanged(const int width, const int height) const
        { SubController::WindowSizeChanged(width, height, simulator); }

    inline void MouseClick(const MouseButton button) const
        { SubController::MouseClick(button); }

    inline void MouseRelease(const MouseButton button) const
        { SubController::MouseRelease(button); }

    inline void MouseMove(const int x, const int y) const
        { SubController::MouseMove(x, y, simulator); }

    inline void ScrollMoved(const int offset) const
        { SubController::ScrollMoved(offset, simulator); }

    void Render() const;

private:
    MillingMachineSim& simulator;
};
