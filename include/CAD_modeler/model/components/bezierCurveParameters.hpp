#pragma once

#include <ecs/coordinator.hpp>

#include <vector>
#include <unordered_map>


class BezierCurveParameter {
public:
    BezierCurveParameter(const std::vector<Entity> controlPoints):
        controlPoints(controlPoints) {}

    inline void AddControlPoint(Entity entity)
        { controlPoints.push_back(entity); }

    bool drawPolygon;

    inline const std::vector<Entity>& ControlPoints() const
        { return controlPoints; }

    std::unordered_map<Entity, HandlerId> handlers;
    HandlerId cameraMovedEvent;

private:
    std::vector<Entity> controlPoints;
};
