#pragma once

#include "c2Surface.hpp"

#include "../equidistanceC2SurfaceSystem.hpp"


namespace interSys
{
    class EquidistanceC2Surface final : public C2Surface {
    public:
        explicit EquidistanceC2Surface(const Coordinator& coordinator, Entity entity);

        ~EquidistanceC2Surface() override = default;

        alg::Vec3 PointOnSurface(float u, float v) override;

        alg::Vec3 PartialDerivativeU(float u, float v) override;

        alg::Vec3 PartialDerivativeV(float u, float v) override;

    private:
        std::shared_ptr<EquidistanceC2SurfaceSystem> system;
        Entity entity;
    };
}
