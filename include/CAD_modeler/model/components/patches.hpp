#pragma once

#include <unordered_map>

#include <ecs/entitiesManager.hpp>
#include <ecs/eventsManager.hpp>

#include "../../utilities/vector2D.hpp"


class Patches {
public:
    virtual ~Patches() = default;

    explicit Patches(const int rows=1, const int cols=1):
        controlPoints(rows, cols) {};

    void SetPoint(const Entity pt, const int row, const int col)
        { controlPoints.At(row, col) = pt; }

    Entity GetPoint(const int row, const int col) const
        { return controlPoints.At(row, col); }

    virtual void AddRowOfPatches() = 0;

    virtual void AddColOfPatches() = 0;

    virtual void DeleteRowOfPatches() = 0;

    virtual void DeleteColOfPatches() = 0;

    unsigned int PointsInRow() const
        { return controlPoints.Rows(); }

    unsigned int PointsInCol() const
        { return controlPoints.Cols(); }

    unsigned int PointsCnt() const
        { return PointsInRow() * PointsInCol(); }

    virtual int PatchesInRow() const = 0;

    virtual int PatchesInCol() const = 0;

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

protected:
    Vector2D<Entity> controlPoints;
};
