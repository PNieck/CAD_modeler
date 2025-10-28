#pragma once

#include "../model/modeler.hpp"
#include "../views/modelerGuiView.hpp"
#include "utils/keyboardKey.hpp"
#include "utils/mouseState.hpp"
#include "utils/modelerState.hpp"
#include "subController.hpp"


class ModelerController: public SubController {
public:
    explicit ModelerController(Modeler& modeler);

    void Update(double dt) override
        { modeler.Update(); }

    void Render() override;

    void MouseClick(MouseButton button) override;
    void MouseMove(int x, int y) override;

    void KeyboardKeyPressed(KeyboardKey key) override;

    [[nodiscard]]
    ModelerState GetModelerState() const
        { return modelerState; }

    void SetModelerState(const ModelerState state)
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
