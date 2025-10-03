#include <gtest/gtest.h>

#include <rootFinding/newtonMethod.hpp>
#include <algebra/vec4.hpp>
#include <cmath>


using namespace root;
using namespace alg;


class FourSameIndependentEquations final : public FunctionToFindRoot {
public:
    Vec4 Value(Vec4 args) override {
        const float v = std::pow(args.X(), 3.f) - std::pow(args.X(), 2.f) + 2.f;

        return Vec4(v);
    }

    Mat4x4 Jacobian(Vec4 args) override {
        const float v = 3.f * std::pow(args.X(), 2.f) - 2.f*args.X();

        return {
              v, 0.f, 0.f, 0.f,
            0.f,   v, 0.f, 0.f,
            0.f, 0.f,   v, 0.f,
            0.f, 0.f, 0.f,   v
        };
    }
};


TEST(NewtonMethodTests, FourSameIndependentEquations) {
    FourSameIndependentEquations fun;

    const Vec4 initSol(-20.f);
    const auto solution = NewtonMethod(fun, initSol, 1e-5);

    ASSERT_TRUE(solution.has_value());

    EXPECT_NEAR(solution.value().X(), -1.f, 1e-4);
    EXPECT_NEAR(solution.value().Y(), -1.f, 1e-4);
    EXPECT_NEAR(solution.value().Z(), -1.f, 1e-4);
    EXPECT_NEAR(solution.value().W(), -1.f, 1e-4);
}
