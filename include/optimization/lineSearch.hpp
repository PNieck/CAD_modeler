#pragma once

#include "functionToOptimize.hpp"


namespace opt
{
    class LineSearchMethod {
    public:
        virtual ~LineSearchMethod() = default;

        virtual float Search(FunctionToOptimize& fun, const std::vector<float>& start, const std::vector<float>& direction) = 0;
    };
}
