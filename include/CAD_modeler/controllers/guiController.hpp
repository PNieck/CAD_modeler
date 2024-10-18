#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"


class GuiController {
public:
    explicit GuiController(GLFWwindow* window);
    ~GuiController();

    void PreFrameRender() const;
    void PostFrameRender() const;

    [[nodiscard]]
    bool WantCaptureMouse() const;
};
