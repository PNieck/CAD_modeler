#pragma once

#include <algebra/vec4.hpp>


namespace interSys
{
    class IntersectionPoint {
    public:
        IntersectionPoint(float u1, float v1, float u2, float v2):
            vec(u1, v1, u2, v2) {}

        explicit IntersectionPoint(alg::Vec4&& vec):
            vec(vec) {}

        IntersectionPoint():
            IntersectionPoint(0.f, 0.f, 0.f, 0.f) {}

        inline float& U1()
            { return vec.X(); }

        inline float& V1()
            { return vec.Y(); }

        inline float& U2()
            { return vec.Z(); }

        inline float& V2()
            { return vec.W(); }

        inline float U1() const
            { return vec.X(); }

        inline float V1() const
            { return vec.Y(); }

        inline float U2() const
            { return vec.Z(); }

        inline float V2() const
            { return vec.W(); }


        inline alg::Vec4& AsVector()
            { return vec; }

    private:
        alg::Vec4 vec;
    };
}
