#pragma once

#include "utils/mouseState.hpp"
#include "subController.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <tuple>


class GlController: public SubController {
public:
    GlController(Model& model, GLFWwindow* window, MainController& controller);

    inline void WindowSizeChanged(int width, int height) { model.ChangeViewportSize(width, height); }

    void MouseClick(MouseButton button);
    inline void MouseRelease(MouseButton button) { mouseState.ButtonReleased(button); }
    void MouseMove(int x, int y);
    void ScrollMoved(int offset);

private:
    static constexpr float ROTATION_COEFF = 0.02f;
    static constexpr float TRANSLATION_COEFF = 0.01f;
    static constexpr float SCROLL_COEFF = 0.7f;

    // FIXME: remove
    GLFWwindow* window;

    MouseState mouseState;

    void MoveCursor() const;
    void Add3DPoint() const;
    void TryToSelectObject() const;

    std::tuple<float, float> MouseToViewportCoordinates() const;
};
