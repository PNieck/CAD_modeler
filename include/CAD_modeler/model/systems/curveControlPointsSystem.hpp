#pragma once

#include <ecs/system.hpp>

#include <memory>
#include <map>
#include "../components/curveControlPoints.hpp"


class CurveControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateControlPoints(const std::vector<Entity>& entities, SystemId system);

    void AddControlPoint(Entity object, Entity controlPoint, SystemId system);

    void DeleteControlPoint(Entity object, Entity controlPoint, SystemId system);

    void MergeControlPoints(Entity curve, Entity oldCP, Entity newCP, SystemId system);

private:
    class DeletionHandler;

    std::map<SystemId, std::shared_ptr<DeletionHandler>> deletionHandlers;

    std::shared_ptr<DeletionHandler> GetDeletionHandler(SystemId systemId);

    class ControlPointMovedHandler: public EventHandler<Position> {
    public:
        ControlPointMovedHandler(Entity targetObject, Coordinator& coordinator):
            coordinator(coordinator), targetObject(targetObject) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
    };


    class DeletionHandler: public EventHandler<CurveControlPoints> {
    public:
        DeletionHandler(Coordinator& coordinator, SystemId systemId):
            coordinator(coordinator), systemId(systemId) {}

        void HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        SystemId systemId;
    };
};
