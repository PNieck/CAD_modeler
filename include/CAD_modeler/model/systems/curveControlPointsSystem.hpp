#pragma once

#include <ecs/system.hpp>

#include <memory>
#include "../components/curveControlPoints.hpp"


class CurveControlPointsSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity CreateControlPoints(const std::vector<Entity>& entities);

    void AddControlPoint(Entity curve, Entity entity);

    void DeleteControlPoint(Entity curve, Entity entity);


private:
    class DeletionHandler;

    std::shared_ptr<DeletionHandler> deletionHandler;


    class ControlPointMovedHandler: public EventHandler<Position> {
    public:
        ControlPointMovedHandler(Entity curve, Coordinator& coordinator):
            coordinator(coordinator), curve(curve) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity curve;
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
