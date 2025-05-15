#pragma once

#include "surfaceSystem.hpp"

#include "../components/position.hpp"
#include "../components/rotation.hpp"
#include "../components/scale.hpp"
#include "../components/torusParameters.hpp"

#include <vector>


class ToriSystem final : public SurfaceSystem {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetParameters(Entity entity, const TorusParameters& params);

    void Render(const alg::Mat4x4& cameraMtx);

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

    float MaxU(Entity e) const override
        { return 2.f * std::numbers::pi_v<float>; }

    float MaxV(Entity e) const override
        { return 2.f * std::numbers::pi_v<float>; }

private:
    void UpdateMesh(Entity e, const TorusParameters& params) const;

    static std::vector<float> GenerateMeshVertices(const TorusParameters& params);
    static std::vector<uint32_t> GenerateMeshIndices(const TorusParameters& params);
};
