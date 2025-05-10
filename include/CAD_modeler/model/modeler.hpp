#pragma once

#include "model.hpp"

#include "managers/saveManager.hpp"

#include "systems/toriSystem.hpp"
#include "systems/gridSystem.hpp"
#include "systems/cursorSystem.hpp"
#include "systems/pointsSystem.hpp"
#include "systems/nameSystem.hpp"
#include "systems/selectionSystem.hpp"
#include "systems/c0CurveSystem.hpp"
#include "systems/c2CurveSystem.hpp"
#include "systems/interpolationCurvesRenderingSystem.hpp"
#include "systems/interpolationCurveSystem.hpp"
#include "systems/c0SurfaceSystem.hpp"
#include "systems/c0CylinderSystem.hpp"
#include "systems/c0PatchesSystem.hpp"
#include "systems/c2SurfacesSystem.hpp"
#include "systems/c2CylinderSystem.hpp"
#include "systems/controlNetSystem.hpp"
#include "systems/controlPointsRegistrySystem.hpp"
#include "systems/gregoryPatchesSystem.hpp"
#include "systems/vectorSystem.hpp"
#include "systems/intersectionsSystem.hpp"

#include "components/scale.hpp"
#include "components/rotation.hpp"

#include "utils/nameGenerator.hpp"

#include "../utilities/line.hpp"


class Modeler final: public Model
{
public:
    Modeler(int viewportWidth, int viewportHeight);

    Entity AddTorus();

    Entity AddC0Curve(const std::vector<Entity>& controlPoints);

    Entity AddC2Curve(const std::vector<Entity>& controlPoints);

    Entity AddInterpolationCurve(const std::vector<Entity>& controlPoints);

    Entity AddC0Surface(const alg::Vec3& direction, float length, float width);

    Entity AddC2Surface(const alg::Vec3& direction, float length, float width);

    Entity AddC0Cylinder();

    Entity AddC2Cylinder();

    void AddControlPointToC0Curve(const Entity curve, const Entity entity) const
        { c0CurveSystem->AddControlPoint(curve, entity); }

    inline void AddControlPointToC2Curve(Entity curve, Entity entity) const
        { c2CurveSystem->AddControlPoint(curve, entity); }

    void MergeControlPoints(Entity e1, Entity e2);

    bool IsAControlPoint(const Entity entity) const
        { return controlPointsRegistrySys->IsAControlPoint(entity); }

    void AddControlPointToInterpolationCurve(const Entity curve, const Entity entity) const
        { coordinator.GetSystem<InterpolationCurveSystem>()->AddControlPoint(curve, entity); }

    void AddRowOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddColOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddRowOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddColOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    inline void DeleteRowOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteColOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteRowOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteColOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    void AddRowOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddColOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddRowOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddColOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    inline void DeleteRowOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0CylinderSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void DeleteColOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0CylinderSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void DeleteRowOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2CylinderSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void DeleteColOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2CylinderSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void RecalculateC0Cylinder(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0CylinderSystem->Recalculate(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void RecalculateC2Cylinder(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2CylinderSystem->Recalculate(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void RecalculateC0Surface(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->Recalculate(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void RecalculateC2Surface(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->Recalculate(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline int GetRowsCntOfC0Patches(Entity surface) const
        { return c0PatchesSystem->GetRowsCnt(surface); }

    inline int GetColsOfC0Patches(Entity surface) const
        { return c0PatchesSystem->GetColsCnt(surface); }

    inline int GetRowsCntOfC2Patches(Entity surface) const
        { return c2SurfaceSystem->GetRowsCnt(surface); }

    inline int GetColsCntOfC2Patches(Entity surface) const
        { return c2SurfaceSystem->GetColsCnt(surface); }

    inline int GetColsCntOfC2Cylinder(Entity cylinder) const
        { return c2CylinderSystem->GetColsCnt(cylinder); }

    inline int GetRowsCntOfC2Cylinder(Entity cylinder) const
        { return c2CylinderSystem->GetRowsCnt(cylinder); }

    inline void DeleteControlPointFromCurve(Entity curve, Entity controlPoint) const
        { c0CurveSystem->DeleteControlPoint(curve, controlPoint); }

    inline const std::unordered_set<Entity>& GetAllC0Surfaces() const
        { return c0SurfaceSystem->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllC2Surfaces() const
        { return c2SurfaceSystem->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllC2Cylinders() const
        { return c2CylinderSystem->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllTori() const
        { return toriSystem->GetEntities(); }

    inline void SetCursorPosition(float x, float y, float z) const
        { cursorSystem->SetPosition(alg::Vec3(x, y, z)); }

    Position GetCursorPosition() const
        { return cursorSystem->GetPosition(); }

    // TODO; Change to normal coordinates
    inline void SetCursorPositionFromViewport(float x, float y)
        { cursorSystem->SetPosition(PointFromViewportCoordinates(x, y)); }

    Entity Add3DPointFromViewport(float x, float y);

    const std::unordered_set<Entity>& EntitiesWithNames() const
        { return nameSystem->EntitiesWithNames(); }

    const Name& GetEntityName(const Entity entity) const
        { return nameSystem->GetName(entity); }

    bool IsSelected(const Entity entity) const
        { return selectionSystem->IsSelected(entity); }

    void Select(const Entity entity) const
        { return selectionSystem->Select(entity); }

    void TryToSelectFromViewport(float x, float y);

    void SelectMultipleFromViewport(float x, float y, float dist);

    void Deselect(const Entity entity) const
        { return selectionSystem->Deselect(entity); }

    inline void DeselectAllEntities() const
        { selectionSystem->DeselectAllEntities(); }

    inline const std::unordered_set<Entity>& GetAllSelectedEntities() const
        { return selectionSystem->GetEntities(); }

    inline const std::set<ComponentId>& GetEntityComponents(Entity entity) const
        { return coordinator.GetEntityComponents(entity); }

    template <typename Comp>
    const Comp& GetComponent(const Entity entity) const
        { return coordinator.GetComponent<Comp>(entity); }

    template <typename Comp>
    void SetComponent(const Entity entity, const Comp& comp)
        { coordinator.SetComponent<Comp>(entity, comp); }

    inline void DeleteEntity(Entity entity)
        { coordinator.DestroyEntity(entity); }

    inline Entity GetMiddlePoint() const
        { return selectionSystem->GetMiddlePoint(); }

    inline void ChangeSelectedEntitiesPosition(const Position& newMidPoint)
        { selectionSystem->MoveSelected(newMidPoint); }

    inline void ChangeSelectedEntitiesScale(const Scale& scale)
        { selectionSystem->ScaleSelected(scale); }

    inline void RotateSelectedEntities(const Rotation& rotation)
        { selectionSystem->RotateSelected(rotation); }

    inline void ChangeEntityName(Entity entity, const Name& name)
        { nameSystem->SetName(entity, name); }

    inline const std::unordered_set<Entity>& GetAllPoints() const
        { return pointsSystem->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllCurves() const
        { return coordinator.GetSystem<CurveControlPointsSystem>()->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllC0Curves() const
        { return c0CurveSystem->GetEntities(); }

    inline const std::unordered_set<Entity>& GetAllC2Curves() const
        { return c2CurveSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllInterpolationCurves() const
        { return coordinator.GetSystem<InterpolationCurveSystem>()->GetEntities(); }

    inline void ShowC2BSplinePolygon(Entity entity) const
        { return c2CurveSystem->ShowBSplinePolygon(entity); }

    void HideC2BSplinePolygon(const Entity entity) const
        { return c2CurveSystem->HideBSplinePolygon(entity); }

    void ShowC2BezierPolygon(const Entity entity) const
        { return c2CurveSystem->ShowBezierPolygon(entity); }

    void HideC2BezierPolygon(const Entity entity) const
        { return c2CurveSystem->HideBezierPolygon(entity); }

    void ShowC2BezierControlPoints(const Entity entity) const
        { return c2CurveSystem->ShowBezierControlPoints(entity); }

    void HideC2BezierControlPoints(const Entity entity) const
        { return c2CurveSystem->HideBezierControlPoints(entity); }

    void SetSurfaceDensity(const Entity entity, const PatchesDensity density) const
        { c0SurfaceSystem->SetDensity(entity, density); }

    void ShowPatchesPolygon(const Entity entity, const Patches& patches) const
        { controlNetSystem->AddControlPointsNet(entity, patches); }

    void HidePatchesPolygon(const Entity entity) const
        { controlNetSystem->DeleteControlPointsNet(entity); }

    bool HasPatchesPolygon(const Entity entity) const
        { return controlNetSystem->HasControlPointsNet(entity); }

    std::vector<GregoryPatchesSystem::Hole> GetHolesPossibleToFill(const std::unordered_set<Entity>& entities) const;
    
    Entity FillHole(const GregoryPatchesSystem::Hole& hole);

    void ShowGregoryNet(const Entity gregoryPatches)
        { gregoryPatchesSystem->ShowControlNet(gregoryPatches); }

    void HideGregoryNet(const Entity gregoryPatches)
        { gregoryPatchesSystem->HideControlNet(gregoryPatches); }

    template <typename Comp>
    static constexpr ComponentId GetComponentId()
        { return ComponentsManager::GetComponentId<Comp>(); }

    void LoadScene(const std::string& path)
        { saveManager.LoadScene(path, coordinator); }

    void SaveScene(const std::string& path)
        { saveManager.SaveScene(path, coordinator); }

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step);

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step, const Position& guidance);

    std::optional<Entity> FindSelfIntersection(Entity e, float step);

    std::optional<Entity> FindSelfIntersection(Entity e, float step, const Position& guidance);

    void TurnIntersectionCurveToInterpolation(Entity curve);

    void ClearScene();

    // TODO: delet
    void ShowDerivativesU(Entity e);
    void ShowDerivativesV(Entity e);
    void ShowC2Normals(Entity e);
    void ShowC2Normals(Entity e, float u, float v);
    void ShowC0Normals(Entity e);
    void ShowC0Normals(Entity e, float u, float v);

    bool selectingEntities = false;
    float selectionCircleX = 0.0f;
    float selectionCircleY = 0.0f;
    float selectionCircleRadius = 0.2f;

    void Update() const;

private:
    std::shared_ptr<ToriSystem> toriSystem;
    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<CursorSystem> cursorSystem;
    std::shared_ptr<PointsSystem> pointsSystem;
    std::shared_ptr<NameSystem> nameSystem;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<C0CurveSystem> c0CurveSystem;
    std::shared_ptr<C2CurveSystem> c2CurveSystem;
    std::shared_ptr<InterpolationCurvesRenderingSystem> interpolationRenderingSystem;
    std::shared_ptr<C0SurfaceSystem> c0SurfaceSystem;
    std::shared_ptr<C0CylinderSystem> c0CylinderSystem;
    std::shared_ptr<C0PatchesSystem> c0PatchesSystem;
    std::shared_ptr<C2SurfaceSystem> c2SurfaceSystem;
    std::shared_ptr<C2CylinderSystem> c2CylinderSystem;
    std::shared_ptr<ControlNetSystem> controlNetSystem;
    std::shared_ptr<ControlPointsRegistrySystem> controlPointsRegistrySys;
    std::shared_ptr<GregoryPatchesSystem> gregoryPatchesSystem;
    std::shared_ptr<VectorSystem> vectorSystem;
    std::shared_ptr<IntersectionSystem> intersectionSystem;

    SaveManager saveManager;
    NameGenerator nameGenerator;

    alg::Vec3 PointFromViewportCoordinates(float x, float y);
    Line LineFromViewportCoordinates(float x, float y);

    std::optional<Entity> SetIntersectionCurveUp(const std::optional<Entity>& curve);

    void RenderSystemsObjects(const alg::Mat4x4& viewMtx, const alg::Mat4x4& persMtx, float nearPlane, float farPlane) const override;
};


template <>
inline void Modeler::SetComponent<TorusParameters>(const Entity entity, const TorusParameters& params)
    { toriSystem->SetParameters(entity, params); }
