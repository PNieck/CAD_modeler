#pragma once

#include <ecs/entitiesManager.hpp>
#include "../../utilities/vector2D.hpp"


class C0Patches {
public:
    static constexpr int RowsInPatch = 4;
    static constexpr int ColsInPatch = 4;
    static constexpr int PointsInPatch = RowsInPatch * ColsInPatch;

    C0Patches(int rows=1, int cols=1):
        controlPoints(3*rows+1, 3*cols+1) {}
    
    inline void SetPoint(Entity pt, int patchRow, int patchCol, int ptRow, int ptCol)
        { controlPoints.At(3*patchRow + ptRow, 3*patchCol + ptCol) = pt; }

    inline void SetPoint(Entity pt, int row, int col)
        { controlPoints.At(row, col) = pt; }

    inline Entity GetPoint(int patchRow, int patchCol, int ptRow, int ptCol) const
        { return controlPoints.At(3*patchRow + ptRow, 3*patchCol + ptCol); }

    inline Entity GetPoint(int row, int col) const
        { return controlPoints.At(row, col); }

    inline void AddRow()
        { controlPoints.AddRows(3); }

    inline void AddCol()
        { controlPoints.AddCols(3); }

    inline void DeleteRow()
        { controlPoints.DeleteRows(3); }

    inline void DeleteCol()
        { controlPoints.DeleteCols(3); }

    inline int PointsInRow() const
        { return controlPoints.Rows(); }

    inline int PointsInCol() const
        { return controlPoints.Cols(); }

    inline int Rows() const
        { return (PointsInRow() - 1) / 3; }

    inline int Cols() const
        { return (PointsInCol() - 1) / 3; }

    inline int PointsCnt() const
        { return PointsInCol() * PointsInRow(); }

    std::unordered_map<Entity, HandlerId> controlPointsHandlers;
    HandlerId deletionHandler;

private:
    Vector2D<Entity> controlPoints;
};
