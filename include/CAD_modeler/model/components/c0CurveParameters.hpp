#pragma once

#include <ecs/coordinator.hpp>

#include <vector>
#include <unordered_map>


class C0CurveParameters {
public:
    C0CurveParameters(const std::vector<Entity> controlPoints):
        controlPoints(controlPoints) {}

    inline void AddControlPoint(Entity entity)
        { controlPoints.push_back(entity); }

    inline void DeleteControlPoint(Entity entity)
        { controlPoints.erase(std::find(controlPoints.begin(), controlPoints.end(), entity)); }

    bool drawPolygon;
    bool meshNeedsUpdate;

    inline const std::vector<Entity>& ControlPoints() const
        { return controlPoints; }

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

private:
    std::vector<Entity> controlPoints;
};
