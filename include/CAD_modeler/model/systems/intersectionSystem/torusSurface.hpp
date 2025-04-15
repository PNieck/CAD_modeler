#pragma once

#include "surfaces.hpp"

#include <ecs/coordinator.hpp>

#include <numbers>


namespace interSys
{
    class TorusSurface final : public Surface {
    public:
        TorusSurface(const Coordinator& coord, const Entity entity):
            params(coord.GetComponent<TorusParameters>(entity)),
            pos(coord.GetComponent<Position>(entity)),
            rot(coord.GetComponent<Rotation>(entity)),
            scale(coord.GetComponent<Scale>(entity))
            {}

        ~TorusSurface() override = default;
    
        alg::Vec3 PointOnSurface(const float u, const float v) override
            { return ToriSystem::PointOnTorus(params, pos, rot, scale, u, v).vec; }
    
        alg::Vec3 PartialDerivativeU(const float u, const float v) override
            { return ToriSystem::PartialDerivativeWithRespectToAlpha(params, rot, scale, u, v); }

        alg::Vec3 PartialDerivativeV(const float u, const float v) override
            { return ToriSystem::PartialDerivativeWithRespectToBeta(params, rot, scale, u, v); }

        float MaxUInitSampleVal() override
            { return 2.f * std::numbers::pi_v<float>; }

        float MaxVInitSampleVal() override
            { return 2.f * std::numbers::pi_v<float>; }

        float MaxU() override
            { return std::numeric_limits<float>::infinity(); }

        float MinU() override
            { return -std::numeric_limits<float>::infinity(); }

        float MaxV() override
            { return std::numeric_limits<float>::infinity(); }

        float MinV() override
            { return -std::numeric_limits<float>::infinity(); }

    private:   
        const TorusParameters& params;
        const Position& pos;
        const Rotation& rot;
        const Scale& scale;
    };
}
