#include <gtest/gtest.h>

#include <algebra/conjugateGradientMethod.hpp>


using namespace alg;


class SimpleTestFunction final : public FunctionToOptimize {
    public:
    float Value(const std::vector<float> &args) override {
        if (args.size() != 2)
            throw std::invalid_argument("Wrong number of arguments");

        return std::powf(args[0] - 2.f, 4) + std::powf(args[0] - 2.f*args[1], 2);
    }

    std::vector<float> Gradient(const std::vector<float> &args) override {
        if (args.size() != 2)
            throw std::invalid_argument("Wrong number of arguments");

        return {
            4.f * std::powf(args[0] - 2.f, 3) + 2.f * (args[0] - 2.f*args[1]),
            -4.f * (args[0] - 2.f*args[1])
        };
    }
};


TEST(ConjungateGradientMethodTests, SimpleFunctionToOptimize) {
    SimpleTestFunction function;

    const auto solution = ConjugationGradientMethod(
        function,
        { 0, 3 },
        0.1f,
        0.00001
    );

    ASSERT_TRUE(solution.has_value());
    ASSERT_EQ(solution.value().size(), 2);

    ASSERT_NEAR(solution.value()[0], 2.f, 0.1f);
    ASSERT_NEAR(solution.value()[1], 1.f, 0.1f);
}
