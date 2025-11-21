#pragma once

#include "surfaceSystem.hpp"
#include "c2PatchesSystem.hpp"

#include <memory>


class EquidistanceC2System : public SurfaceSystem {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init();

    Entity AddSurface(Entity base, float dist);

    Position PointOnSurface(Entity e, float u, float v) const override;

    alg::Vec3 PartialDerivativeU(Entity e, float u, float v) const override;
    alg::Vec3 PartialDerivativeV(Entity e, float u, float v) const override;

    alg::Vec3 NormalVector(Entity e, float u, float v) const override;

    float MaxU(Entity e) const override;

    float MaxV(Entity e) const override;

private:
    std::shared_ptr<C2PatchesSystem> c2PatchesSystem;
};
