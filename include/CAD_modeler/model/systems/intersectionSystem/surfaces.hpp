#pragma once

#include "../toriSystem.hpp"

#include <algebra/vec3.hpp>

#include <ecs/coordinator.hpp>

#include <numbers>


namespace interSys
{
    class Surface {
    public:
        virtual alg::Vec3 PointOnSurface(float u, float v) = 0;
        
        virtual alg::Vec3 PartialDerivativeU(float u, float v) = 0;
        virtual alg::Vec3 PartialDerivativeV(float u, float v) = 0;

        alg::Vec3 NormalVector(float u, float v)
        {
            const alg::Vec3 tangent1 = PartialDerivativeU(u, v);
            const alg::Vec3 tangent2 = PartialDerivativeV(u, v);

            return alg::Cross(tangent1, tangent2).Normalize();
        }

        virtual float MaxU() = 0;
        virtual float MaxV() = 0;
    };
    
    
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
