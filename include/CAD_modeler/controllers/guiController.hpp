#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "subController.hpp"


class GuiController: public SubController {
public:
    GuiController(Model& model, MainController& controller);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

};
