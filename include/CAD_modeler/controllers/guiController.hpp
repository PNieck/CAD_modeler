#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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

    template <typename Comp>
    inline void ChangeComponent(Entity entity, const Comp& comp)
        { model.SetComponent<Comp>(entity, comp); }

    inline void DeleteEntity(Entity entity) const
        { model.DeleteEntity(entity); }

    inline void TranslateSelected(const Position& newMidPoint)
        { model.ChangeSelectedEntitiesPosition(newMidPoint); }

    inline void ScaleSelected(const Scale& scale)
        { model.ChangeSelectedEntitiesScale(scale); }

    inline void RotateSelected(const Rotation& rot)
        { model.RotateSelectedEntities(rot); }
};
