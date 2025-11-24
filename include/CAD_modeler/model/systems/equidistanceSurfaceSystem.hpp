#pragma once

#include "surfaceSystem.hpp"

#include <memory>


class EquidistanceSurfaceSystem : public SurfaceSystem {
public:
    Entity AddSurface(Entity base, float dist);

    Position PointOnSurface(Entity e, float u, float v) const override;

    alg::Vec3 NormalVector(Entity e, float u, float v) const override;

    float MaxU(Entity e) const override;

    float MaxV(Entity e) const override;

protected:
    static alg::Vec3 PartialDerivativeU(
        float dist,
        const alg::Vec3& partialU,
        const alg::Vec3& partialV,
        const alg::Vec3& partialUU,
        const alg::Vec3& partialUV
    );

    static alg::Vec3 PartialDerivativeV(
        float dist,
        const alg::Vec3& partialU,
        const alg::Vec3& partialV,
        const alg::Vec3& partialVV,
        const alg::Vec3& partialUV
    );

    alg::Vec3 PartialDerivativeUApprox(Entity e, float u, float v) const;
    alg::Vec3 PartialDerivativeVApprox(Entity e, float u, float v) const;

    std::shared_ptr<SurfaceSystem> baseSystem;
};
