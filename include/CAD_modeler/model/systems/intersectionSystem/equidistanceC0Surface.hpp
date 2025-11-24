#pragma once

#include "c0Surface.hpp"

#include "../equidistanceC0SurfaceSystem.hpp"


namespace interSys
{
    class EquidistanceC0Surface final : public C0Surface {
    public:
        explicit EquidistanceC0Surface(const Coordinator& coordinator, Entity entity);

        ~EquidistanceC0Surface() override = default;

        alg::Vec3 PointOnSurface(float u, float v) override;

        alg::Vec3 PartialDerivativeU(float u, float v) override;

        alg::Vec3 PartialDerivativeV(float u, float v) override;

    private:
        std::shared_ptr<EquidistanceC0SurfaceSystem> system;
        Entity entity;
    };
}
