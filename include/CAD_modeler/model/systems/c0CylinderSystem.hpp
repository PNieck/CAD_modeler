#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/patchesDensity.hpp"
#include "c0PatchesSystem.hpp"
#include "controlNetSystem.hpp"


class C0CylinderSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateCylinder(const Position& pos, const alg::Vec3& direction, float radius);
    
    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float radius) const;

    inline void MergeControlPoints(Entity cylinder, Entity oldCP, Entity newCP)
        { coordinator->GetSystem<C0PatchesSystem>()->MergeControlPoints(cylinder, oldCP, newCP, Coordinator::GetSystemID<C0CylinderSystem>()); }

    void ShowBezierPolygon(Entity cylinder) const;

    inline void HideBezierPolygon(Entity cylinder) const
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(cylinder); }

    inline void SetDensity(Entity entity, PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetSystem<C0PatchesSystem>()->GetRowsCnt(surface); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetSystem<C0PatchesSystem>()->GetColsCnt(surface); }

    void Recalculate(Entity cylinder, const Position& pos, const alg::Vec3& direction, float radius) const;

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    NameGenerator nameGenerator;

    class DeletionHandler: public EventHandler<C0Patches> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const C0Patches& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };

    class ControlPointMovedHandler: public EventHandler<Position> {
    public:
        ControlPointMovedHandler(Entity targetObject, Coordinator& coordinator):
            coordinator(coordinator), targetObject(targetObject) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
    };
};
