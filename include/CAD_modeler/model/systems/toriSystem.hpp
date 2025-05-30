#pragma once

#include "surfaceSystem.hpp"

#include "../components/position.hpp"
#include "../components/rotation.hpp"
#include "../components/scale.hpp"
#include "../components/torusParameters.hpp"


class ToriSystem final : public SurfaceSystem {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetParameters(Entity entity, const TorusParameters& params);

    Position PointOnSurface(Entity torus, float u, float v) const override;

    static alg::Vec3 PointOnSurface(const TorusParameters& params, float u, float v);

    static Position PointOnSurface(
        const TorusParameters& params,
        const Position& pos,
        const Rotation& rot,
        const Scale& scale,
        float u,
        float v
    );

    static alg::Vec3 PartialDerivativeU(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float u,
        float v
    );

    [[nodiscard]]
    alg::Vec3 PartialDerivativeU(Entity e, float u, float v) const override;

    static alg::Vec3 PartialDerivativeV(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float alpha,
        float beta
    );
    alg::Vec3 PartialDerivativeV(Entity e, float u, float v) const override;

    static alg::Vec3 NormalVector(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float alpha,
        float beta
    );
    alg::Vec3 NormalVector(Entity e, float u, float v) const override;

    static constexpr float MaxU()
        { return 2.f * std::numbers::pi_v<float>; }

    float MaxU(Entity e) const override
        { return MaxU(); }

    static constexpr float MaxV()
        { return 2.f * std::numbers::pi_v<float>; }

    float MaxV(Entity e) const override
        { return MaxV(); }
};
