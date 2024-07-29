#pragma once

#include "c2Patches.hpp"


class C2CylinderPatches: public C2Patches {
public:
    C2CylinderPatches(int rows=1, int cols=1):
        C2Patches(rows, cols+2) {}

    int PatchesInCol() const override
        { return controlPoints.Cols() - 5; }
};
