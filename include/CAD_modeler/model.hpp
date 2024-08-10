#pragma once

#include <ecs/coordinator.hpp>

#include "model/cameraManager.hpp"

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
#include "model/systems/c2SurfacesSystem.hpp"
#include "model/systems/c2CylinderSystem.hpp"
#include "model/systems/controlNetSystem.hpp"

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

    inline Entity AddC0Surface(const alg::Vec3& direction, float length, float width) const
        { return c0SurfaceSystem->CreateSurface(cursorSystem->GetPosition(), direction, length, width); }

    inline Entity AddC2Surface(const alg::Vec3& direction, float length, float width) const
        { return c2SurfaceSystem->CreateSurface(cursorSystem->GetPosition(), direction, length, width); }

    inline Entity AddC0Cylinder() const
        { return c0CylinderSystem->CreateCylinder(cursorSystem->GetPosition(), alg::Vec3(0.f, 1.f, 0.f), 1.f); }

    inline Entity AddC2Cylinder() const
        { return c2CylinderSystem->CreateCylinder(cursorSystem->GetPosition(), alg::Vec3(0.f, 1.f, 0.f), 1.f); }

    inline void AddControlPointToCurve(Entity curve, Entity entity) const
        { c0CurveSystem->AddControlPoint(curve, entity); }

    inline void AddRowOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->AddRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void AddColOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->AddColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void AddRowOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->AddRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void AddColOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->AddColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteRowOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteColOfC0SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c0SurfaceSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteRowOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->DeleteRowOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void DeleteColOfC2SurfacePatches(Entity surface, const alg::Vec3& direction, float length, float width) const
        { c2SurfaceSystem->DeleteColOfPatches(surface, cursorSystem->GetPosition(), direction, length, width); }

    inline void AddRowOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0CylinderSystem->AddRowOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void AddColOfC0CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c0CylinderSystem->AddColOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void AddRowOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2CylinderSystem->AddRowOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

    inline void AddColOfC2CylinderPatches(Entity surface, float radius = 1.f, const alg::Vec3& dir = alg::Vec3(0.f, 1.f, 0.f)) const
        { c2CylinderSystem->AddColOfPatches(surface, cursorSystem->GetPosition(), dir, radius); }

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

    void ChangeViewportSize(int width, int height);

    inline void SetCursorPosition(float x, float y, float z) const
        { cursorSystem->SetPosition(alg::Vec3(x, y, z)); }

    // TODO; Change to normal coordinates
    inline void SetCursorPositionFromViewport(float x, float y)
        { cursorSystem->SetPosition(PointFromViewportCoordinates(x, y)); }

    Entity Add3DPointFromViewport(float x, float y);

    inline const std::unordered_set<Entity> EntitiesWithNames() const
        { return nameSystem->EntitiesWithNames(); }

    inline const Name& GetEntityName(Entity entity) const
        { return nameSystem->GetName(entity); }

    inline bool IsSelected(Entity entity) const
        { return selectionSystem->IsSelected(entity); }

    inline void Select(Entity entity)
        { return selectionSystem->Select(entity); }

    void TryToSelectFromViewport(float x, float y);

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
        { return coordinator.GetSystem<CurveControlPointsSystem>()->GetEntities(); }

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

    inline const void SetSurfaceDensity(Entity entity, PatchesDensity density) const
        { c0SurfaceSystem->SetDensity(entity, density); }

    inline const void ShowPatchesPolygon(Entity entity, const Patches& patches) const
        { controlNetSystem->AddControlPointsNet(entity, patches); }

    inline const void HidePatchesPolygon(Entity entity) const
        { controlNetSystem->DeleteControlPointsNet(entity); }

    inline bool HasPatchesPolygon(Entity entity) const
        { return controlNetSystem->HasControlPointsNet(entity); }

    template <typename Comp>
    static inline constexpr ComponentId GetComponentId()
        { return ComponentsManager::GetComponentId<Comp>(); }

    CameraManager cameraManager;

private:
    
    Coordinator coordinator;
    ShaderRepository shadersRepo;

    std::shared_ptr<ToriSystem> toriSystem;
    std::shared_ptr<GridSystem> gridSystem;
    std::shared_ptr<CursorSystem> cursorSystem;
    std::shared_ptr<PointsSystem> pointsSystem;
    std::shared_ptr<NameSystem> nameSystem;
    std::shared_ptr<SelectionSystem> selectionSystem;
    std::shared_ptr<C0CurveSystem> c0CurveSystem;
    std::shared_ptr<C2CurveSystem> c2CurveSystem;
    std::shared_ptr<InterpolationCurveSystem> interpolationCurveSystem;
    std::shared_ptr<C0SurfaceSystem> c0SurfaceSystem;
    std::shared_ptr<C0CylinderSystem> c0CylinderSystem;
    std::shared_ptr<C0PatchesSystem> c0PatchesSystem;
    std::shared_ptr<C2SurfaceSystem> c2SurfaceSystem;
    std::shared_ptr<C2CylinderSystem> c2CylinderSystem;
    std::shared_ptr<ControlNetSystem> controlNetSystem;

    alg::Vec3 PointFromViewportCoordinates(float x, float y);
    Line LineFromViewportCoordinates(float x, float y);

    void RenderAnaglyphsFrame();

    void RenderPerspectiveFrame();

    void RenderSystemsObjects(const alg::Mat4x4& viewMtx, const alg::Mat4x4& persMtx, float nearPlane, float farPlane) const;
};
