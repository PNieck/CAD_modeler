#include <gtest/gtest.h>

#include <numbers>

#include <CAD_modeler/utilities/angle.hpp>


TEST(AngleTests, DegreesToRadians) {
    const float eps = 0.001;

    EXPECT_NEAR(Angle::FromDegrees(0.f).ToRadians(), 0, eps);
    EXPECT_NEAR(Angle::FromDegrees(90.f).ToRadians(), std::numbers::pi_v<float> / 2.f, eps);
    EXPECT_NEAR(Angle::FromDegrees(180.f).ToRadians(), std::numbers::pi_v<float>, eps);
    EXPECT_NEAR(Angle::FromDegrees(360.f).ToRadians(), 2.f * std::numbers::pi_v<float>, eps);
}
