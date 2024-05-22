#pragma once

#include <ecs/coordinator.hpp>

#include <vector>


class C0SurfacePatches {
public:
    static constexpr int RowsInPatch = 4;
    static constexpr int ColsInPatch = 4;

    C0SurfacePatches(int rows=1, int cols=1);
    
    inline void SetPoint(Entity pt, int patchRow, int patchCol, int ptRow, int ptCol)
        { controlPoints[3*patchRow + ptRow][3*patchCol + ptCol] = pt; }

    inline Entity GetPoint(int patchRow, int patchCol, int ptRow, int ptCol) const
        { return controlPoints[3*patchRow + ptRow][3*patchCol + ptCol]; }

    void AddRow();

    void AddCol();

    void DeleteRow();

    void DeleteCol();

    inline int Rows() const
        { return patchesRows; }

    inline int Cols() const
        { return patchesCols; }

    inline int PointsInRow() const
        { return 3 * patchesRows + 1; }

    inline int PointsInCol() const
        { return 3 * patchesCols + 1; }

    inline int PointsCnt() const
        { return PointsInCol() * PointsInRow(); }

private:
    int patchesRows;
    int patchesCols;

    std::vector<std::vector<Entity>> controlPoints;
};
