#pragma once

#include <vector>
#include <algebra/vec2.hpp>


enum MouseButton {
    Right = 0,
    Middle = 1,
    Left = 2
};


class MouseState {
public:
    MouseState();

    inline void ButtonClicked(MouseButton button) { buttonClicked[button] = true; }
    inline void ButtonReleased(MouseButton button) { buttonClicked[button] = false; }
    
    inline bool IsButtonClicked(MouseButton button) const { return buttonClicked[button]; }

    void Moved(int x, int y);

    inline alg::IVec2 TranslationGet() const { return actMousePos - oldMousePos; }
    inline alg::IVec2 PositionGet() const { return actMousePos; }

private:
    static const int BUTTON_CNT = 3;

    bool buttonClicked[BUTTON_CNT];

    alg::IVec2 oldMousePos;
    alg::IVec2 actMousePos;
};
