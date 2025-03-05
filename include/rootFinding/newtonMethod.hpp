#pragma once

#include "functionToFindRoot.hpp"

#include <optional>


namespace root
{
    std::optional<alg::Vec4> NewtonMethod(FunctionToFindRoot& fun, const alg::Vec4& initSol, float eps, int maxIter = 100);
}