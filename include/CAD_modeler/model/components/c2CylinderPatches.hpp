#pragma once

#include "c2Patches.hpp"


class C2CylinderPatches final : public C2Patches {
public:
    explicit C2CylinderPatches(const int rows=1, const int cols=1):
        C2Patches(rows, cols+2) {}

    int PatchesInCol() const override
        { return controlPoints.Cols() - 5; }
};
