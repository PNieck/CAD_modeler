#pragma once

#include "utils/mouseState.hpp"
#include "../model.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


class GlController {
public:
    GlController(Model& model, GLFWwindow* window);

    inline void WindowSizeChanged(int width, int height) { model.ChangeViewportSize(width, height); }

    void MouseClick(MouseButton button);
    inline void MouseRelease(MouseButton button) { mouseState.ButtonReleased(button); }
    void MouseMove(int x, int y);
    void ScrollMoved(int offset);

private:
    static constexpr float ROTATION_COEFF = 0.02f;
    static constexpr float TRANSLATION_COEFF = 0.01f;
    static constexpr float SCROLL_COEFF = 0.7f;

    GLFWwindow* window;

    Model& model;

    MouseState mouseState;
};
