#pragma once

#include <ecs/system.hpp>

#include <memory>
#include <map>
#include "../components/curveControlPoints.hpp"
#include "../components/position.hpp"


class CurveControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateControlPoints(const std::vector<Entity>& cps, SystemId system);

    void AddControlPoint(Entity object, Entity controlPoint, SystemId system);

    void DeleteControlPoint(Entity object, Entity controlPoint, SystemId system);

    void MergeControlPoints(Entity curve, Entity oldCP, Entity newCP, SystemId system);

private:
    class DeletionHandler;

    std::map<SystemId, std::shared_ptr<DeletionHandler>> deletionHandlers;

    std::shared_ptr<DeletionHandler> GetDeletionHandler(SystemId systemId);

    class ControlPointMovedHandler final : public EventHandler<Position> {
    public:
        ControlPointMovedHandler(const Entity targetObject, Coordinator& coordinator, const SystemId sysId):
            coordinator(coordinator), targetObject(targetObject), sysId(sysId) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
        SystemId sysId;
    };


    class DeletionHandler final : public EventHandler<CurveControlPoints> {
    public:
        DeletionHandler(Coordinator& coordinator, const SystemId systemId):
            coordinator(coordinator), systemId(systemId) {}

        void HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        SystemId systemId;
    };
};
