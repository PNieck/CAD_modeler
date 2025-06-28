#pragma once

#include "CAD_modeler/model/components/position.hpp"
#include "ecs/system.hpp"


class SurfaceSystem : public System {
public:
    virtual Position PointOnSurface(Entity e, float u, float v) const = 0;

    virtual alg::Vec3 PartialDerivativeU(Entity e, float u, float v) const = 0;
    virtual alg::Vec3 PartialDerivativeV(Entity e, float u, float v) const = 0;

    virtual alg::Vec3 NormalVector(const Entity e, const float u, const float v) const
    {
        const alg::Vec3 tangent1 = PartialDerivativeU(e, u, v);
        const alg::Vec3 tangent2 = PartialDerivativeU(e, u, v);

        return Cross(tangent1, tangent2);
    }

    // virtual float MinU(Entity e) const
    //     { return 0.f; }

    virtual float MaxU(Entity e) const = 0;

    // virtual float MinV(Entity e) const
    //     { return 0.f; }

    virtual float MaxV(Entity e) const = 0;
};
