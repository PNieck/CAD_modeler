#pragma once

#include "utils/mouseState.hpp"
#include "../model/model.hpp"


class SubController {
protected:
    static constexpr float ROTATION_COEF = 0.02f;
    static constexpr float TRANSLATION_COEF = 0.01f;
    static constexpr float SCROLL_COEF = 0.7f;

    static MouseState mouseState;

    inline static void WindowSizeChanged(const int width, const int height, Model& model)
        { model.ChangeViewportSize(width, height); }

    inline static void MouseRelease(const MouseButton button)
        { mouseState.ButtonReleased(button); }

    inline static void MouseClick(const MouseButton button)
        { mouseState.ButtonClicked(button); }

    static void MouseMove(int x, int y, Model& model);

    static void ScrollMoved(int offset, Model& model);
};
