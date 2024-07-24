#pragma once

#include <imgui.h>

#include "subController.hpp"
#include "utils/curvesTypes.hpp"
#include "utils/surfacesTypes.hpp"


class GuiController: public SubController {
public:
    GuiController(Model& model, MainController& controller);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

    inline void AddTorus() const
        { model.AddTorus(); }

    Entity AddCurve(const std::vector<Entity>& entities, CurveType curveType) const;

    Entity AddSurface(SurfaceType surfaceType, const alg::Vec3& dir, float length, float width) const;

    inline Entity AddC0Cylinder() const
        { return model.AddC0Cylinder(); }

    void AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    inline void AddRowOfC0CylinderPatches(Entity surface, const alg::Vec3& dir, float radius)
        { return model.AddRowOfC0CylinderPatches(surface, radius, dir); }

    inline void AddColOfC0CylinderPatches(Entity surface, const alg::Vec3& dir, float radius)
        { return model.AddColOfC0CylinderPatches(surface, radius, dir); }

    inline void DeleteRowOfC0CylinderPatches(Entity surface, const alg::Vec3& dir, float radius)
        { return model.DeleteRowOfC0CylinderPatches(surface, radius, dir); }

    inline void DeleteColOfC0CylinderPatches(Entity surface, const alg::Vec3& dir, float radius)
        { return model.DeleteColOfC0CylinderPatches(surface, radius, dir); }

    inline void RecalculateC0Cylinder(Entity cylinder, const alg::Vec3& dir, float radius)
        { return model.RecalculateC0CylinderPatches(cylinder, radius, dir); }

    void RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

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

    inline void SetNewSurfaceDensity(Entity entity, C0PatchesDensity newDensity)
        { model.SetSurfaceDensity(entity, newDensity); }

    inline void ShowPatchesPolygon(Entity entity)
        { model.ShowPatchesPolygon(entity); }

    inline void HidePatchesPolygon(Entity entity)
        { model.HidePatchesPolygon(entity); }
};
