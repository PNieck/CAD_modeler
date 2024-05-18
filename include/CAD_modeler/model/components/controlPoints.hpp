#pragma once

#include <ecs/coordinator.hpp>
#include <ecs/eventHandler.hpp>

#include <vector>
#include <unordered_map>

#include "position.hpp"


class ControlPoints {
public:
    ControlPoints(const std::vector<Entity> controlPoints):
        controlPoints(controlPoints) {}

    inline void AddControlPoint(Entity entity)
        { controlPoints.push_back(entity); }

    inline void DeleteControlPoint(Entity entity)
        { controlPoints.erase(std::find(controlPoints.begin(), controlPoints.end(), entity)); }

    inline const std::vector<Entity>& GetPoints() const
        { return controlPoints; }

    inline size_t Size() const
        { return controlPoints.size(); }

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

private:
    std::vector<Entity> controlPoints;
};
