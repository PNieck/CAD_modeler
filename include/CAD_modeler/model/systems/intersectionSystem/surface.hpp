#pragma once

#include <algebra/vec3.hpp>


namespace interSys
{
    class Surface {
    public:
        virtual ~Surface() = default;

        virtual alg::Vec3 PointOnSurface(float u, float v) = 0;
        
        virtual alg::Vec3 PartialDerivativeU(float u, float v) = 0;
        virtual alg::Vec3 PartialDerivativeV(float u, float v) = 0;

        alg::Vec3 NormalVector(const float u, const float v)
        {
            const alg::Vec3 tangent1 = PartialDerivativeU(u, v);
            const alg::Vec3 tangent2 = PartialDerivativeV(u, v);

            return Cross(tangent1, tangent2).Normalize();
        }

        virtual void Normalize(float& u, float& v) {}

        virtual float MinUSampleVal()
            { return 0.f; }

        virtual float MaxUSampleVal()
            { return 1.0f; }

        virtual float MinVSampleVal()
            { return 0.f; }

        virtual float MaxVSampleVal()
            { return 1.0f; }

        virtual float MaxU() = 0;
        virtual float MinU() = 0;

        virtual float MaxV() = 0;
        virtual float MinV() = 0;
    };
}
