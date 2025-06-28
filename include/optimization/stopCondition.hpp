#pragma once

#include "functionToOptimize.hpp"

#include <vector>


namespace opt {
    class StopCondition {
    public:
        virtual ~StopCondition() = default;

        virtual bool ShouldStop(FunctionToOptimize& fun, const std::vector<float>& args) = 0;
    };
}

