#pragma once

#include "patches.hpp"


class C0Patches: public Patches {
public:
    static constexpr int RowsInPatch = 4;
    static constexpr int ColsInPatch = 4;
    static constexpr int PointsInPatch = RowsInPatch * ColsInPatch;

    C0Patches(int rows=1, int cols=1):
        Patches(3*rows+1, 3*cols+1) {}

    void AddRowOfPatches() override
        { controlPoints.AddRows(3); }

    void AddColOfPatches() override
        { controlPoints.AddCols(3); }

    void DeleteRowOfPatches() override
        { controlPoints.DeleteRows(3); }

    void DeleteColOfPatches() override
        { controlPoints.DeleteCols(3); }

    int PatchesInRow() const override
        { return (controlPoints.Rows() - 1) / 3; }

    int PatchesInCol() const override
        {  return (controlPoints.Cols() - 1) / 3; }
};
