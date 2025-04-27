#pragma once

#include <vector>
#include <optional>

#include "lineSearch.hpp"
#include "stopCondition.hpp"


namespace opt {
    std::optional<std::vector<float>> ConjugateGradientMethod(
        FunctionToOptimize& fun,
        LineSearchMethod& lineSearch,
        const std::vector<float> &initSol,
        unsigned int maxIt,
        StopCondition& stopCondition
    );
};
