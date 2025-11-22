#pragma once

#include "c2Surface.hpp"

#include "../equidistanceC2SurfaceSystem.hpp"


namespace interSys
{
    class EquidistanceSystem final : public C2Surface {
    public:
        explicit EquidistanceSystem(const Coordinator& coordinator, Entity entity);

        ~EquidistanceSystem() override = default;

        alg::Vec3 PointOnSurface(float u, float v) override;

        alg::Vec3 PartialDerivativeU(float u, float v) override;

        alg::Vec3 PartialDerivativeV(float u, float v) override;

    private:
        std::shared_ptr<EquidistanceC2System> system;
        Entity entity;
    };
}
