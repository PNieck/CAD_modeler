#pragma once

#include <vector>
#include <optional>

#include "lineSearchMethods/lineSearch.hpp"


namespace opt {
    

    std::optional<std::vector<float>> ConjugateGradientMethod(FunctionToOptimize& fun, LineSearchMethod& lineSearch, const std::vector<float> &initSol, float eps, unsigned int maxIt);
};
