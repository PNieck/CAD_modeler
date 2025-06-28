#pragma once

#include "../../components/c0Patches.hpp"
#include "../../components/position.hpp"

#include <algebra/vec3.hpp>

#include <ecs/coordinator.hpp>


class SingleC0Patch {
public:
    SingleC0Patch(Coordinator& coord, const C0Patches &patches, float u, float v);

    [[nodiscard]]
    const alg::Vec3& Point(const int row, const int col) const
        { return coordinator.GetComponent<Position>(patches.GetPoint(firstRow + row, firstCol + col)).vec; }

private:
    Coordinator& coordinator;

    const C0Patches& patches;

    int firstRow, firstCol;
};
