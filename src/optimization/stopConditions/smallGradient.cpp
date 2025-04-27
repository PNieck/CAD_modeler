#include <optimization/stopConditions/smallGradient.hpp>

#include <optimization/utils.hpp>


bool opt::SmallGradient::ShouldStop(FunctionToOptimize &fun, const std::vector<float>& args)
{
    return LengthSquared(fun.Gradient(args)) < eps;
}
