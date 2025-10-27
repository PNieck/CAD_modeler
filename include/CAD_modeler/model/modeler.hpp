#pragma once

#include "model.hpp"

#include "managers/saveManager.hpp"

#include "systems/toriSystem.hpp"
#include "systems/toriRenderingSystem.hpp"
#include "systems/trimmedToriRenderingSystem.hpp"
#include "systems/gridSystem.hpp"
#include "systems/cursorSystem.hpp"
#include "systems/pointsSystem.hpp"
#include "systems/nameSystem.hpp"
#include "systems/selectionSystem.hpp"
#include "systems/c0CurveSystem.hpp"
#include "systems/c2CurveSystem.hpp"
#include "systems/interpolationCurvesRenderingSystem.hpp"
#include "systems/interpolationCurveSystem.hpp"
#include "systems/c0PatchesSystem.hpp"
#include "systems/c0PatchesRenderSystem.hpp"
#include "systems/trimmedC0PatchesRenderSystem.hpp"
#include "systems/c2PatchesSystem.hpp"
#include "systems/c2PatchesRenderSystem.hpp"
#include "systems/trimmedC2PatchesRenderSystem.hpp"
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

    Entity AddC0Plane(const alg::Vec3& direction, float length, float width);

    Entity AddC2Plane(const alg::Vec3& direction, float length, float width);

    Entity AddC0Cylinder();

    Entity AddC2Cylinder();

    void AddControlPointToC0Curve(const Entity curve, const Entity entity) const
        { c0CurveSystem->AddControlPoint(curve, entity); }

    void AddControlPointToC2Curve(const Entity curve, const Entity entity) const
        { c2CurveSystem->AddControlPoint(curve, entity); }

    void MergeControlPoints(Entity e1, Entity e2);

    bool IsAControlPoint(const Entity entity) const
        { return controlPointsRegistrySys->IsAControlPoint(entity); }

    void AddControlPointToInterpolationCurve(const Entity curve, const Entity entity) const
        { coordinator.GetSystem<InterpolationCurveSystem>()->AddControlPoint(curve, entity); }

    void AddRowOfC0PlanePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddColOfC0PlanePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddRowOfC2PlanePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void AddColOfC2PlanePatches(Entity surface, const alg::Vec3& direction, float length, float width);

    void DeleteRowOfC0PlanePatches(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c0PatchesSystem->DeleteRowOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    void DeleteColOfC0PlanePatches(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c0PatchesSystem->DeleteColOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    void DeleteRowOfC2PlanePatches(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c2PatchesSystem->DeleteRowOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    void DeleteColOfC2PlanePatches(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c2PatchesSystem->DeleteColOfPlanePatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    void AddRowOfC0CylinderPatches(Entity cylinder, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddColOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddRowOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void AddColOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f));

    void DeleteRowOfC0CylinderPatches(const Entity surface, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0PatchesSystem->DeleteRowOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    void DeleteColOfC0CylinderPatches(const Entity surface, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0PatchesSystem->DeleteColOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    void DeleteRowOfC2CylinderPatches(const Entity cylinder, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2PatchesSystem->DeleteRowOfCylinderPatches(cylinder, cursorSystem->GetPosition(), dir, radius); }

    void DeleteColOfC2CylinderPatches(const Entity surface, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2PatchesSystem->DeleteColOfCylinderPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    void RecalculateC0Cylinder(const Entity surface, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0PatchesSystem->RecalculateCylinder(surface, cursorSystem->GetPosition(), dir, radius); }

    void RecalculateC2Cylinder(const Entity surface, const float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2PatchesSystem->RecalculateCylinder(surface, cursorSystem->GetPosition(), dir, radius); }

    void RecalculateC0Plane(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c0PatchesSystem->RecalculatePlane(surface, cursorSystem->GetPosition(), direction, length, width); }

    void RecalculateC2Plane(const Entity surface, const alg::Vec3& direction, const float length, const float width) const
        { c2PatchesSystem->RecalculatePlane(surface, cursorSystem->GetPosition(), direction, length, width); }

    int GetRowsCntOfC0Patches(const Entity surface) const
        { return c0PatchesSystem->GetRowsCnt(surface); }

    int GetColsOfC0Patches(const Entity surface) const
        { return c0PatchesSystem->GetColsCnt(surface); }

    int GetRowsCntOfC2Patches(const Entity surface) const
        { return c2PatchesSystem->GetRowsCnt(surface); }

    int GetColsCntOfC2Patches(const Entity surface) const
        { return c2PatchesSystem->GetColsCnt(surface); }

    void DeleteControlPointFromCurve(const Entity curve, const Entity controlPoint) const
        { c0CurveSystem->DeleteControlPoint(curve, controlPoint); }

    const std::unordered_set<Entity>& GetAllC0Surfaces() const
        { return c0PatchesSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllC2Surfaces() const
        { return c2PatchesSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllTori() const
        { return toriSystem->GetEntities(); }

    void SetCursorPosition(const float x, const float y, const float z) const
        { cursorSystem->SetPosition(alg::Vec3(x, y, z)); }

    Position GetCursorPosition() const
        { return cursorSystem->GetPosition(); }

    // TODO; Change to normal coordinates
    void SetCursorPositionFromViewport(float x, float y)
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

    void DeselectAllEntities() const
        { selectionSystem->DeselectAllEntities(); }

    const std::unordered_set<Entity>& GetAllSelectedEntities() const
        { return selectionSystem->GetEntities(); }

    const std::set<ComponentId>& GetEntityComponents(const Entity entity) const
        { return coordinator.GetEntityComponents(entity); }

    template <typename Comp>
    const Comp& GetComponent(const Entity entity) const
        { return coordinator.GetComponent<Comp>(entity); }

    template <typename Comp>
    void SetComponent(const Entity entity, const Comp& comp)
        { coordinator.SetComponent<Comp>(entity, comp); }

    void DeleteEntity(const Entity entity)
        { coordinator.DestroyEntity(entity); }

    Entity GetMiddlePoint() const
        { return selectionSystem->GetMiddlePoint(); }

    void ChangeSelectedEntitiesPosition(const Position& newMidPoint)
        { selectionSystem->MoveSelected(newMidPoint); }

    void ChangeSelectedEntitiesScale(const Scale& scale)
        { selectionSystem->ScaleSelected(scale); }

    void RotateSelectedEntities(const Rotation& rotation)
        { selectionSystem->RotateSelected(rotation); }

    void ChangeEntityName(const Entity entity, const Name& name)
        { nameSystem->SetName(entity, name); }

    const std::unordered_set<Entity>& GetAllPoints() const
        { return pointsSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllCurves() const
        { return coordinator.GetSystem<CurveControlPointsSystem>()->GetEntities(); }

    const std::unordered_set<Entity>& GetAllC0Curves() const
        { return c0CurveSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllC2Curves() const
        { return c2CurveSystem->GetEntities(); }

    const std::unordered_set<Entity>& GetAllInterpolationCurves() const
        { return coordinator.GetSystem<InterpolationCurveSystem>()->GetEntities(); }

    void ShowC2BSplinePolygon(const Entity entity) const
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

    void SetSurfaceDensity(const Entity entity, const PatchesDensity density)
        { coordinator.SetComponent(entity, density); }

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

    void LoadScene(const std::string& path);

    void SaveScene(const std::string& path)
        { saveManager.SaveScene(path, coordinator); }

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step);

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step, const Position& guidance);

    std::optional<Entity> FindSelfIntersection(Entity e, float step);

    std::optional<Entity> FindSelfIntersection(Entity e, float step, const Position& guidance);

    Entity TurnIntersectionCurveToInterpolation(Entity curve);

    void ClearScene();

    void FillTrimmingRegion(Entity e, size_t u, size_t v);
    void DrawPointOnUV(Entity e, size_t u, size_t v);
    void ApplyTrimming(Entity e);
    void ClearTrimming(Entity e);

    bool& Points3DRendering()
        { return pointsSystem->renderPoints; }

    // TODO: delete
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
    std::shared_ptr<ToriRenderingSystem> toriRenderingSystem;
    std::shared_ptr<TrimmedToriRenderingSystem> trimmedToriRenderingSystem;

    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<CursorSystem> cursorSystem;
    std::shared_ptr<PointsSystem> pointsSystem;
    std::shared_ptr<NameSystem> nameSystem;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<C0CurveSystem> c0CurveSystem;
    std::shared_ptr<C2CurveSystem> c2CurveSystem;
    std::shared_ptr<InterpolationCurvesRenderingSystem> interpolationRenderingSystem;

    std::shared_ptr<C0PatchesSystem> c0PatchesSystem;
    std::shared_ptr<C0PatchesRenderSystem> c0PatchesRenderSystem;
    std::shared_ptr<TrimmedC0PatchesRenderSystem> trimmedC0PatchesRenderSystem;

    std::shared_ptr<C2PatchesSystem> c2PatchesSystem;
    std::shared_ptr<C2PatchesRenderSystem> c2PatchesRenderSystem;
    std::shared_ptr<TrimmedC2PatchesRenderSystem> trimmedC2PatchesRenderSystem;

    std::shared_ptr<ControlNetSystem> controlNetSystem;
    std::shared_ptr<ControlPointsRegistrySystem> controlPointsRegistrySys;
    std::shared_ptr<GregoryPatchesSystem> gregoryPatchesSystem;
    std::shared_ptr<VectorSystem> vectorSystem;
    std::shared_ptr<IntersectionSystem> intersectionSystem;

    SaveManager saveManager;
    NameGenerator nameGenerator;

    alg::Vec3 PointFromViewportCoordinates(float x, float y);
    Line LineFromViewportCoordinates(float x, float y);

    void SetIntersectionCurveUp(Entity curve, Entity e1, Entity e2);

    void RenderSystemsObjects(const alg::Mat4x4& viewMtx, const alg::Mat4x4& persMtx, float nearPlane, float farPlane) const override;
};


template <>
inline void Modeler::SetComponent<TorusParameters>(const Entity entity, const TorusParameters& params) {
    toriSystem->SetParameters(entity, params);

    if (toriRenderingSystem->HasEntity(entity))
        toriRenderingSystem->Update(entity);

    if (trimmedToriRenderingSystem->HasEntity(entity))
        trimmedToriRenderingSystem->Update(entity);
}
