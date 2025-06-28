#pragma once

#include <algebra/vec4.hpp>
#include <algebra/mat4x4.hpp>


namespace root
{
    class FunctionToFindRoot {
    public:
        virtual ~FunctionToFindRoot() = default;

        virtual alg::Vec4 Value(alg::Vec4 args) = 0;
        virtual alg::Mat4x4 Jacobian(alg::Vec4 args) = 0;
    };
}
