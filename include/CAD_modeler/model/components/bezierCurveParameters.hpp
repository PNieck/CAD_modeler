#pragma once

#include <ecs/entitiesManager.hpp>

#include <vector>


class BezierCurveParameter {
public:
    BezierCurveParameter(const std::vector<Entity> controlPoints):
        controlPoints(controlPoints) {}

    inline void AddControlPoint(Entity entity)
        { controlPoints.push_back(entity); }

    bool drawPolygon;

    inline const std::vector<Entity>& ControlPoints() const
        { return controlPoints; }

private:
    std::vector<Entity> controlPoints;
};
