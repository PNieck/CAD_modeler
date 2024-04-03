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

    inline void AddTorus() const
        { model.AddTorus(); }

    inline void SelectEntity(Entity entity) const
        { model.Select(entity); }

    inline void DeselectEntity(Entity entity) const
        { model.Deselect(entity); }

    inline void ChangePosition(Entity entity, const Position& pos)
        { model.SetPosition(entity, pos); }
};
