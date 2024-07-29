#pragma once

#include <ecs/system.hpp>

#include "utils/nameGenerator.hpp"
#include "../components/position.hpp"
#include "../components/curveControlPoints.hpp"
#include "../components/c0Patches.hpp"
#include "../components/patchesDensity.hpp"
#include "c0PatchesSystem.hpp"
#include "controlNetSystem.hpp"


class C0SurfaceSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width);

    void AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    void DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;
    void DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

    inline void SetDensity(Entity entity, PatchesDensity density) const
        { coordinator->SetComponent<PatchesDensity>(entity, density); }

    void ShowBezierNet(Entity cylinder) const;

    inline void HideBezierNet(Entity surface) const
        { coordinator->GetSystem<ControlNetSystem>()->DeleteControlPointsNet(surface); }

    inline int GetRowsCnt(Entity surface) const
        { return coordinator->GetSystem<C0PatchesSystem>()->GetRowsCnt(surface); }

    inline int GetColsCnt(Entity surface) const
        { return coordinator->GetSystem<C0PatchesSystem>()->GetColsCnt(surface); }

    void Recalculate(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const;

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
