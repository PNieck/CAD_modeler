#pragma once

#include "surfaces.hpp"

#include <ecs/coordinator.hpp>

#include <numbers>


namespace interSys
{
    class TorusSurface: public Surface {
    public:
        TorusSurface(const Coordinator& coord, const Entity entity):
            params(coord.GetComponent<TorusParameters>(entity)),
            pos(coord.GetComponent<Position>(entity)),
            rot(coord.GetComponent<Rotation>(entity)),
            scale(coord.GetComponent<Scale>(entity))
            {}
    
        alg::Vec3 PointOnSurface(float u, float v) override
            { return ToriSystem::PointOnTorus(params, pos, rot, scale, u, v).vec; }
    
        alg::Vec3 PartialDerivativeU(float u, float v) override
            { return ToriSystem::PartialDerivativeWithRespectToAlpha(params, rot, scale, u, v); }

        alg::Vec3 PartialDerivativeV(float u, float v) override
            { return ToriSystem::PartialDerivativeWithRespectToBeta(params, rot, scale, u, v); }

        float MaxU() override
            { return 2.f * std::numbers::pi_v<float>; }

        float MaxV() override
            { return 2.f * std::numbers::pi_v<float>; }

    private:   
        const TorusParameters& params;
        const Position& pos;
        const Rotation& rot;
        const Scale& scale;
    };
}
