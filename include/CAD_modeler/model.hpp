#pragma once

#include <ecs/coordinator.hpp>

#include "model/systems/cameraSystem.hpp"
#include "model/systems/toriSystem.hpp"
#include "model/systems/gridSystem.hpp"
#include "model/systems/cursorSystem.hpp"
#include "model/systems/pointsSystem.hpp"
#include "model/systems/nameSystem.hpp"
#include "model/systems/selectionSystem.hpp"
#include "model/systems/c0CurveSystem.hpp"
#include "model/systems/c2CurveSystem.hpp"
#include "model/systems/interpolationCurveSystem.hpp"
#include "model/systems/c0SurfaceSystem.hpp"
#include "model/systems/c0CylinderSystem.hpp"
#include "model/systems/c0PatchesSystem.hpp"

#include "model/systems/shaders/shaderRepository.hpp"

#include "model/components/scale.hpp"
#include "model/components/rotation.hpp"

#include "utilities/line.hpp"


class Model
{
public:
    Model(int viewport_width, int viewport_height);

    void RenderFrame();

    void AddTorus();

    inline Entity AddC0Curve(const std::vector<Entity>& controlPoints) const
        { return c0CurveSystem->CreateC0Curve(controlPoints); }

    inline Entity AddC2Curve(const std::vector<Entity>& controlPoints) const
        { return c2CurveSystem->CreateC2Curve(controlPoints); }

    inline Entity AddInterpolationCurve(const std::vector<Entity>& controlPoints) const
        { return interpolationCurveSystem->CreateCurve(controlPoints); }

    inline Entity AddC0Surface() const
        { return c0surfaceSystem->CreateSurface(cursorSystem->GetPosition()); }

    inline void AddControlPointToCurve(Entity curve, Entity entity) const
        { c0CurveSystem->AddControlPoint(curve, entity); }

    inline void AddRowOfC0SurfacePatches(Entity surface) const
        { c0surfaceSystem->AddRowOfPatches(surface); }

    inline void AddColOfC0SurfacePatches(Entity surface) const
        { c0surfaceSystem->AddColOfPatches(surface); }

    inline void DeleteRowOfC0SurfacePatches(Entity surface) const
        { c0surfaceSystem->DeleteRowOfPatches(surface); }

    inline void DeleteColOfC0SurfacePatches(Entity surface) const
        { c0surfaceSystem->DeleteColOfPatches(surface); }

    inline int GetRowsCntOfC0Patches(Entity surface) const
        { return c0surfaceSystem->GetRowsCnt(surface); }

    inline int GetColsOfC0Patches(Entity surface) const
        { return c0surfaceSystem->GetColsCnt(surface); }

    inline void DeleteControlPointFromCurve(Entity curve, Entity controlPoint) const
        { c0CurveSystem->DeleteControlPoint(curve, controlPoint); }

    inline void RotateCamera(float x, float y) const
        { cameraSys->RotateAroundTarget(x, y); }

    inline float GetCameraDistanceFromTarget() const
        { return cameraSys->GetDistanceToTarget(); }

    inline void SetCameraDistance(float newDist) const
        { cameraSys->SetDistanceToTarget(newDist); }

    inline void MultiplyCameraDistanceFromTarget(float coefficient) const
        { cameraSys->MultiplyDistanceToTarget(coefficient); }

    void ChangeViewportSize(int width, int height);

    inline void SetCursorPosition(float x, float y, float z) const
        { cursorSystem->SetPosition(alg::Vec3(x, y, z)); }

    // TODO; Change to normal coordinates
    void SetCursorPositionFromViewport(float x, float y) const;

    Entity Add3DPointFromViewport(float x, float y) const;

    inline const std::unordered_set<Entity> EntitiesWithNames() const
        { return nameSystem->EntitiesWithNames(); }

    inline const Name& GetEntityName(Entity entity) const
        { return nameSystem->GetName(entity); }

    inline bool IsSelected(Entity entity) const
        { return selectionSystem->IsSelected(entity); }

    inline void Select(Entity entity)
        { return selectionSystem->Select(entity); }

    void TryToSelectFromViewport(float x, float y) const;

    inline void Deselect(Entity entity)
        { return selectionSystem->Deselect(entity); }

    inline void DeselectAllEntities() const
        { selectionSystem->DeselectAllEntities(); }

    inline const std::unordered_set<Entity>& GetAllSelectedEntities() const
        { return selectionSystem->GetEntities(); }

    inline const std::set<ComponentId>& GetEntityComponents(Entity entity) const
        { return coordinator.GetEntityComponents(entity); }

    template <typename Comp>
    inline const Comp& GetComponent(Entity entity) const
        { return coordinator.GetComponent<Comp>(entity); }

    template <typename Comp>
    inline void SetComponent(Entity entity, const Comp& comp)
        { coordinator.SetComponent<Comp>(entity, comp); }

    template <>
    inline void SetComponent<TorusParameters>(Entity entity, const TorusParameters& params)
        { toriSystem->SetTorusParameter(entity, params); }

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
        { return coordinator.GetSystem<ControlPointsSystem>()->GetEntities(); }

    inline const void ShowC2BSplinePolygon(Entity entity) const
        { return c2CurveSystem->ShowBSplinePolygon(entity); }

    inline const void HideC2BSplinePolygon(Entity entity) const
        { return c2CurveSystem->HideBSplinePolygon(entity); }

    inline const void ShowC2BezierPolygon(Entity entity) const
        { return c2CurveSystem->ShowBezierPolygon(entity); }

    inline const void HideC2BezierPolygon(Entity entity) const
        { return c2CurveSystem->HideBezierPolygon(entity); }

    inline const void ShowC2BezierControlPoints(Entity entity) const
        { return c2CurveSystem->ShowBezierControlPoints(entity); }

    inline const void HideC2BezierControlPoints(Entity entity) const
        { return c2CurveSystem->HideBezierControlPoints(entity); }

    inline const void SetSurfaceDensity(Entity entity, C0SurfaceDensity density) const
        { c0surfaceSystem->SetDensity(entity, density); }

    template <typename Comp>
    static inline constexpr ComponentId GetComponentId()
        { return ComponentsManager::GetComponentId<Comp>(); }

private:
    Coordinator coordinator;
    ShaderRepository shadersRepo;

    std::shared_ptr<CameraSystem> cameraSys;
    std::shared_ptr<ToriSystem> toriSystem;
    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<CursorSystem> cursorSystem;
    std::shared_ptr<PointsSystem> pointsSystem;
    std::shared_ptr<NameSystem> nameSystem;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<C0CurveSystem> c0CurveSystem;
    std::shared_ptr<C2CurveSystem> c2CurveSystem;
    std::shared_ptr<InterpolationCurveSystem> interpolationCurveSystem;
    std::shared_ptr<C0SurfaceSystem>  c0surfaceSystem;
    std::shared_ptr<C0CylinderSystem> c0CylinderSystem;
    std::shared_ptr<C0PatchesSystem> c0PatchesSystem;

    alg::Vec3 PointFromViewportCoordinates(float x, float y) const;
    Line LineFromViewportCoordinates(float x, float y) const;
};
