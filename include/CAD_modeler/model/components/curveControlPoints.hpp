#pragma once

#include <ecs/coordinator.hpp>

#include <vector>
#include <unordered_map>


class CurveControlPoints {
public:
    explicit CurveControlPoints(const std::vector<Entity>& controlPoints):
        controlPoints(controlPoints) {}

    void AddControlPoint(const Entity entity)
        { controlPoints.push_back(entity); }

    void DeleteControlPoint(const Entity entity)
        { controlPoints.erase(std::ranges::find(controlPoints, entity)); }

    void SwapControlPoint(const Entity oldCP, const Entity newCP)
        { std::ranges::replace(controlPoints, oldCP, newCP); }

    const std::vector<Entity>& GetPoints() const
        { return controlPoints; }

    size_t Size() const
        { return controlPoints.size(); }

    bool Empty() const
        { return controlPoints.empty(); }

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

private:
    std::vector<Entity> controlPoints;
};
