#pragma once

#include <unordered_map>

#include <ecs/entitiesManager.hpp>
#include <ecs/eventsManager.hpp>

#include "../../utilities/vector2D.hpp"


class Patches {
public:
    virtual ~Patches() = default;

    Patches(int rows=1, int cols=1):
        controlPoints(rows, cols) {};

    inline void SetPoint(Entity pt, int row, int col)
        { controlPoints.At(row, col) = pt; }

    inline Entity GetPoint(int row, int col) const
        { return controlPoints.At(row, col); }

    virtual void AddRowOfPatches() = 0;

    virtual void AddColOfPatches() = 0;

    virtual void DeleteRowOfPatches() = 0;

    virtual void DeleteColOfPatches() = 0;

    inline int PointsInRow() const
        { return controlPoints.Rows(); }

    inline int PointsInCol() const
        { return controlPoints.Cols(); }

    inline int PointsCnt() const
        { return PointsInRow() * PointsInCol(); }

    virtual int PatchesInRow() const = 0;

    virtual int PatchesInCol() const = 0;

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

protected:
    Vector2D<Entity> controlPoints;
};
