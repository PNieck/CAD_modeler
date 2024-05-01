#pragma once

#include <ecs/coordinator.hpp>
#include <ecs/eventHandler.hpp>

#include <vector>
#include <unordered_map>

#include "position.hpp"


class CurveControlPoints {
public:
    CurveControlPoints(const std::vector<Entity> controlPoints):
        controlPoints(controlPoints) {}

    inline void AddControlPoint(Entity entity)
        { controlPoints.push_back(entity); }

    inline void DeleteControlPoint(Entity entity)
        { controlPoints.erase(std::find(controlPoints.begin(), controlPoints.end(), entity)); }

    inline const std::vector<Entity>& ControlPoints() const
        { return controlPoints; }

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

private:
    std::vector<Entity> controlPoints;
};


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
