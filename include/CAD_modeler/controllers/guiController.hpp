#pragma once

#include <imgui.h>

#include "subController.hpp"
#include "utils/curvesTypes.hpp"


class GuiController: public SubController {
public:
    GuiController(Model& model, MainController& controller);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

    inline void AddTorus() const
        { model.AddTorus(); }

    Entity AddCurve(const std::vector<Entity>& entities, CurveType curveType) const;

    inline void AddControlPointToCurve(Entity curve, Entity entity) const
        { model.AddControlPointToCurve(curve, entity); }

    inline void DeleteControlPointFromCurve(Entity curve, Entity controlPoint) const
        { model.DeleteControlPointFromCurve(curve, controlPoint); }

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

    inline void HideC2BezierControlPoints(Entity entity)
        { model.HideC2BezierControlPoints(entity); }

    inline void ShowC2BezierControlPoints(Entity entity)
        { model.ShowC2BezierControlPoints(entity); }

    inline void HideC2BezierPolygon(Entity entity)
        { model.HideC2BezierPolygon(entity); }

    inline void SetNewSurfaceDensity(Entity entity, C0SurfaceDensity newDensity)
        { model.SetSurfaceDensity(entity, newDensity); }
};
