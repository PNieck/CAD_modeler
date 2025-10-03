#include <gtest/gtest.h>

#include <optimization/conjugateGradientMethod.hpp>

#include <cmath>

#include "optimization/lineSearchMethods/dichotomyLineSearch.hpp"
#include "optimization/stopConditions/smallGradient.hpp"


using namespace opt;


class SimpleTestFunction final : public FunctionToOptimize {
    public:
    float Value(const std::vector<float> &args) override {
        if (args.size() != 2)
            throw std::invalid_argument("Wrong number of arguments");

        return std::pow(args[0] - 2.f, 4.f) + std::pow(args[0] - 2.f*args[1], 2.f);
    }

    std::vector<float> Gradient(const std::vector<float> &args) override {
        if (args.size() != 2)
            throw std::invalid_argument("Wrong number of arguments");

        return {
            4.f * std::pow(args[0] - 2.f, 3.f) + 2.f * (args[0] - 2.f*args[1]),
            -4.f * (args[0] - 2.f*args[1])
        };
    }
};


TEST(ConjungateGradientMethodTests, SimpleFunctionToOptimize) {
    SimpleTestFunction function;
    auto lineSearch = DichotomyLineSearch(0.f, 10.f, 1e-7);
    auto stopCondition = SmallGradient(1e-5);

    const auto solution = ConjugateGradientMethod(
        function,
        lineSearch,
        { 0, 3 },
        1000,
        stopCondition
    );

    ASSERT_TRUE(solution.has_value());
    ASSERT_EQ(solution.value().size(), 2);

    ASSERT_NEAR(solution.value()[0], 2.f, 0.1f);
    ASSERT_NEAR(solution.value()[1], 1.f, 0.1f);
}
