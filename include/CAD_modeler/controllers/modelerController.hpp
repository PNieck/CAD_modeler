#pragma once

#include "../model/modeler.hpp"
#include "../views/modeler/modelerGuiView.hpp"
#include "utils/keyboardKey.hpp"
#include "utils/mouseState.hpp"
#include "utils/modelerState.hpp"
#include "subController.hpp"


class ModelerController: public SubController {
public:
    explicit ModelerController(Modeler& modeler);

    void Update() const
        { modeler.Update(); }

    void Render();

    void WindowSizeChanged(const int width, const int height) const
        { SubController::WindowSizeChanged(width, height, modeler); }

    void MouseClick(MouseButton button);
    void MouseMove(int x, int y) const;
    void ScrollMoved(const int offset) const
        { SubController::ScrollMoved(offset, modeler); }

    void MouseRelease(const MouseButton button) const
        { SubController::MouseRelease(button); }
    void KeyboardKeyPressed(KeyboardKey key);

    [[nodiscard]]
    ModelerState GetModelerState() const
        { return modelerState; }

    inline void SetModelerState(const ModelerState state)
        { modelerState = state; }

private:
    Modeler& modeler;
    ModelerGuiView guiView;
    ModelerState modelerState;

    void Move3DCursor();
    void Add3DPoint();
    void TryToSelectObject();

    [[nodiscard]]
    std::tuple<float, float> MouseToViewportCoordinates() const;
};
