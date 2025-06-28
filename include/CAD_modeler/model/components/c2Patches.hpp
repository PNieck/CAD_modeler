#pragma once

#include "patches.hpp"


class C2Patches: public Patches {
public:
    static constexpr int RowsInPatch = 4;
    static constexpr int ColsInPatch = 4;
    static constexpr int PointsInPatch = RowsInPatch * ColsInPatch;

    explicit C2Patches(const int rows=1, const int cols=1):
        Patches(RowsInPatch + rows - 1, ColsInPatch + cols - 1) {}

    void AddRowOfPatches() override
        { controlPoints.AddRow(); }

    void AddColOfPatches() override
        { controlPoints.AddCol(); }

    void DeleteRowOfPatches() override
        { controlPoints.DeleteRow(); }

    void DeleteColOfPatches() override
        { controlPoints.DeleteCol(); }

    int PatchesInRow() const override
        { return controlPoints.Rows() - 3; }

    int PatchesInCol() const override
        { return controlPoints.Cols() - 3; }
};
