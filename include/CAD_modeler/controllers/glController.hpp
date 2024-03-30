#ifndef GL_CONTROLLER_H
#define GL_CONTROLLER_H

#include "utils/mouseState.hpp"
#include "../model.hpp"


class GlController {
public:
    GlController(Model& model);

    inline void WindowSizeChanged(int width, int height) { model.ChangeViewportSize(width, height); }

    inline void MouseClick(MouseButton button) { mouseState.ButtonClicked(button); }
    inline void MouseRelease(MouseButton button) { mouseState.ButtonReleased(button); }
    void MouseMove(int x, int y);
    void ScrollMoved(int offset);

private:
    static constexpr float ROTATION_COEFF = 0.02f;
    static constexpr float TRANSLATION_COEFF = 0.01f;
    static constexpr float SCROLL_COEFF = 0.7f;

    Model& model;

    MouseState mouseState;
};


#endif