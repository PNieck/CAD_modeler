#pragma once

#include <ecs/coordinator.hpp>

#include "model/systems/cameraSystem.hpp"
#include "model/systems/toriSystem.hpp"
#include "model/systems/gridSystem.hpp"
#include "model/systems/cursorSystem.hpp"
#include "model/systems/pointsSystem.hpp"
#include "model/systems/nameSystem.hpp"
#include "model/systems/selectionSystem.hpp"
#include "model/systems/bezierCurveSystem.hpp"

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
        { return bezierCurveSystem->CreateBezierCurve(controlPoints); }

    inline void AddC0CurveControlPoint(Entity curve, Entity entity) const
        { bezierCurveSystem->AddControlPoint(curve, entity); }

    inline void DeleteC0CurveControlPoint(Entity curve, Entity controlPoint) const
        { bezierCurveSystem->DeleteControlPoint(curve, controlPoint); }

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
        { cursorSystem->SetPosition(glm::vec3(x, y, z)); }

    // TODO; Change to normal coordinates
    void SetCursorPositionFromViewport(float x, float y) const;

    void Add3DPointFromViewport(float x, float y) const;

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

    inline const std::unordered_set<Entity>& SelectedEntities() const
        { return selectionSystem->SelectedEntities(); }

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
        { return pointsSystem->GetPoints(); } 

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
    std::shared_ptr<BezierCurveSystem> bezierCurveSystem;

    glm::vec3 PointFromViewportCoordinates(float x, float y) const;
    Line LineFromViewportCoordinates(float x, float y) const;
};
