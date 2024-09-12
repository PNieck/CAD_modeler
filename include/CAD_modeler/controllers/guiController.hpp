#pragma once

#include <imgui.h>

#include "subController.hpp"
#include "utils/curvesTypes.hpp"
#include "utils/surfacesTypes.hpp"
#include "utils/cylinderTypes.hpp"


class GuiController: public SubController {
public:
    GuiController(Model& model, MainController& controller);

    inline bool WantCaptureMouse()
        { return ImGui::GetIO().WantCaptureMouse; }

    inline void LoadScene(const std::string& path)
        { model.LoadScene(path); }

    inline void SetEyeSeparation(float newValue)
        { model.cameraManager.SetEyeSeparation(newValue); }

    inline void SetConvergence(float newValue)
        { model.cameraManager.SetConvergence(newValue); }

    using CameraType = CameraManager::CameraType;
    inline void SetCameraType(CameraType type)
        { model.cameraManager.SetCameraType(type); }

    inline void FillHole(const GregoryPatchesSystem::Hole& hole)
        { model.FillHole(hole); }

    inline void ShowGregoryNet(Entity gregoryPatches)
        { model.ShowGregoryNet(gregoryPatches); }

    inline void HideGregoryNet(Entity gregoryPatches)
        { model.HideGregoryNet(gregoryPatches); }

    inline void AddTorus() const
        { model.AddTorus(); }

    inline void MergeControlPoints(Entity e1, Entity e2)
        { model.MergeControlPoints(e1, e2); }

    Entity AddCurve(const std::vector<Entity>& entities, CurveType curveType) const;

    Entity AddSurface(SurfaceType surfaceType, const alg::Vec3& dir, float length, float width) const;

    Entity AddCylinder(CylinderType cylinderType) const;

    void AddRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteRowOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void DeleteColOfSurfacePatches(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void AddColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void DeleteRowOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void DeleteColOfCylinderPatches(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void RecalculateCylinder(Entity cylinder, CylinderType cylinderType, const alg::Vec3& dir, float radius);

    void RecalculateSurface(Entity surface, SurfaceType surfaceType, const alg::Vec3& dir, float length, float width);

    void AddControlPointToCurve(Entity curve, Entity entity, CurveType curveType) const;

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

    inline void SetNewSurfaceDensity(Entity entity, PatchesDensity newDensity)
        { model.SetSurfaceDensity(entity, newDensity); }

    inline void ShowPatchesPolygon(Entity entity, const Patches& patches)
        { model.ShowPatchesPolygon(entity, patches); }

    inline void HidePatchesPolygon(Entity entity)
        { model.HidePatchesPolygon(entity); }
};
