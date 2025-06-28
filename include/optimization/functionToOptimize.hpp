#pragma once

#include <vector>


namespace opt
{
    class FunctionToOptimize {
    public:
        virtual ~FunctionToOptimize() = default;

        virtual float Value(const std::vector<float>& args) = 0;
        virtual std::vector<float> Gradient(const std::vector<float>& args) = 0;
    };
}
