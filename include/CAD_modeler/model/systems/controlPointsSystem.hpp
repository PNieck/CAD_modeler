#pragma once

#include <ecs/system.hpp>

#include <memory>
#include "../components/controlPoints.hpp"


class ControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    // TODO: delete
    Entity CreateControlPoints(const std::vector<Entity>& entities);

    void AddControlPoint(Entity object, Entity controlPoint);

    void DeleteControlPoint(Entity object, Entity controlPoint);


private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;


    class ControlPointMovedHandler: public EventHandler<Position> {
    public:
        ControlPointMovedHandler(Entity targetObject, Coordinator& coordinator):
            coordinator(coordinator), targetObject(targetObject) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity targetObject;
    };


    class DeletionHandler: public EventHandler<ControlPoints> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const ControlPoints& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };
};
