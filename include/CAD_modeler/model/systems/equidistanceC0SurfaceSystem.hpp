#pragma once

#include "equidistanceSurfaceSystem.hpp"


class EquidistanceC0SurfaceSystem : public EquidistanceSurfaceSystem {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    alg::Vec3 PartialDerivativeU(Entity e, float u, float v) const override;
    alg::Vec3 PartialDerivativeV(Entity e, float u, float v) const override;
};

