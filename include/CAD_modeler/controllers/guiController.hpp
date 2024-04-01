#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../model.hpp"
#include "utils/mouseState.hpp"


class GuiController {
public:
    GuiController(Model& model);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

    void TorusBigRadiusChanged(float newRadius);

    void TorusSmallRadiusChanged(float newRadius);

private:
    Model& model;
};
