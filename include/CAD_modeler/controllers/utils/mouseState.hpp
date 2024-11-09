#pragma once

#include <algebra/vec2.hpp>


enum MouseButton {
    Right = 0,
    Middle = 1,
    Left = 2
};


class MouseState {
public:
    MouseState();

    inline void ButtonClicked(const MouseButton button)
        { buttonClicked[button] = true; }
    inline void ButtonReleased(const MouseButton button)
        { buttonClicked[button] = false; }

    [[nodiscard]]
    bool IsButtonClicked(const MouseButton button) const
        { return buttonClicked[button]; }

    void Moved(int x, int y);

    [[nodiscard]]
    alg::IVec2 TranslationGet() const
        { return actMousePos - oldMousePos; }

    [[nodiscard]]
    alg::IVec2 PositionGet() const
        { return actMousePos; }

private:
    static constexpr int BUTTON_CNT = 3;

    bool buttonClicked[BUTTON_CNT] = {};

    alg::IVec2 oldMousePos;
    alg::IVec2 actMousePos;
};
