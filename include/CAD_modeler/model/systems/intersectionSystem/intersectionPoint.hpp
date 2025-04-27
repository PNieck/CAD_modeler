#pragma once

#include <algebra/vec4.hpp>


namespace interSys
{
    class IntersectionPoint {
    public:
        IntersectionPoint(const float u1, const float v1, const float u2, const float v2):
            vec(u1, v1, u2, v2) {}

        explicit IntersectionPoint(const alg::Vec4& vec):
            vec(vec) {}

        IntersectionPoint():
            IntersectionPoint(0.f, 0.f, 0.f, 0.f) {}

        float& U1()
            { return vec.X(); }

        float& V1()
            { return vec.Y(); }

        float& U2()
            { return vec.Z(); }

        float& V2()
            { return vec.W(); }

        [[nodiscard]]
        float U1() const
            { return vec.X(); }

        [[nodiscard]]
        float V1() const
            { return vec.Y(); }

        [[nodiscard]]
        float U2() const
            { return vec.Z(); }

        [[nodiscard]]
        float V2() const
            { return vec.W(); }


        alg::Vec4& AsVector()
            { return vec; }

        [[nodiscard]]
        const alg::Vec4& AsVector() const
            { return vec; }

    private:
        alg::Vec4 vec;
    };
}
