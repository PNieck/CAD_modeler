#pragma once

#include "../toriSystem.hpp"

#include <algebra/vec3.hpp>


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
}
