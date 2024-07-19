#pragma once

#include <ecs/system.hpp>

#include <memory>
#include <unordered_map>
#include "../components/curveControlPoints.hpp"


class CurveControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    // TODO: delete
    Entity CreateControlPoints(const std::vector<Entity>& entities);

    void AddControlPoint(Entity object, Entity controlPoint);

    void DeleteControlPoint(Entity object, Entity controlPoint);

    inline bool IsAControlPoint(Entity entity) const
        { return numberOfObjectsConnectedToControlPoint.contains(entity); }

private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;
    std::unordered_map<Entity, unsigned int> numberOfObjectsConnectedToControlPoint;

    void RegisterControlPoint(Entity controlPoint);

    void UnregisterControlPoint(Entity controlPoint);

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
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const CurveControlPoints& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };
};
