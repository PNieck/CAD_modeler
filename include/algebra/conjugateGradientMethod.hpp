#pragma once

#include <vector>
#include <optional>


namespace alg {
    class FunctionToOptimize {
    public:
        virtual ~FunctionToOptimize() = default;

        virtual float Value(const std::vector<float>& args) = 0;
        virtual std::vector<float> Gradient(const std::vector<float>& args) = 0;
        virtual bool CheckDomain(const std::vector<float>& args) {
            (void)args;

            return true;
        }
    };

    std::optional<std::vector<float>> ConjugationGradientMethod(
        FunctionToOptimize& functionToOptimize,
        const std::vector<float>& solutionEstimation,
        float step,
        float eps
    );
};
