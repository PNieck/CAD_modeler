#pragma once

#include "vec4.hpp"
#include "mat4x4.hpp"


namespace alg {
    class FunctionToFindRoot {
    public:
        virtual ~FunctionToFindRoot() = default;

        virtual Vec4 Value(Vec4 args) = 0;
        virtual Mat4x4 Jacobian(Vec4 args) = 0;
    };

    std::optional<Vec4> NewtonMethod(FunctionToFindRoot& fun, const Vec4& initSol, float eps, int maxIter = 100);
};
