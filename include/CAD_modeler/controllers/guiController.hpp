#pragma once

#include <imgui.h>

#include "subController.hpp"


class GuiController: public SubController {
public:
    GuiController(Model& model, MainController& controller);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

    inline void AddTorus() const
        { model.AddTorus(); }

    inline Entity AddC0Curve(const std::vector<Entity>& entities) const
        { return model.AddC0Curve(entities); }

    inline void AddC0CurveControlPoint(Entity curve, Entity entity) const
        { model.AddC0CurveControlPoint(curve, entity); }

    inline void DeleteC0ControlPoint(Entity curve, Entity controlPoint) const
        { model.DeleteC0CurveControlPoint(curve, controlPoint); }

    inline void SelectEntity(Entity entity) const
        { model.Select(entity); }

    inline void DeselectEntity(Entity entity) const
        { model.Deselect(entity); }

    inline void DeselectAllEntities() const
        { model.DeselectAllEntities(); }

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

    inline void ChangeEntityName(Entity entity, const Name& name)
        { model.ChangeEntityName(entity, name); }

    inline void ShowC2BSplinePolygon(Entity entity)
        { model.ShowC2BSplinePolygon(entity); }

    inline void HideC2BSplinePolygon(Entity entity)
        { model.HideC2BSplinePolygon(entity); }

    inline void ShowC2BezierPolygon(Entity entity)
        { model.ShowC2BezierPolygon(entity); }

    inline void HideC2BezierPolygon(Entity entity)
        { model.HideC2BezierPolygon(entity); }
};
