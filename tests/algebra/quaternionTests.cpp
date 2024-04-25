#include <gtest/gtest.h>

#include <algebra/quat.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/quaternion_float.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>


TEST(QuaternionTests, RotationMatricesComparison) {
    alg::Quat algQuat(1.0, 2.0, 3.0, 4.0);
    glm::quat glmQuat(4.0, 1.0, 2.0, 3.0);

    algQuat = algQuat.Normalize();
    glmQuat = glm::normalize(glmQuat);

    auto algMat = algQuat.ToRotationMatrix();
    auto glmMat = glm::toMat4(glmQuat);

    for (int row=0; row < alg::Mat4x4::Rows; ++row) {
        for (int col=0; col < alg::Mat4x4::Cols; ++col) {
            std::cout << algMat(row, col) << " ";
        }
        std::cout << "\n";
    }

    std::cout << glm::to_string(glmMat);

    for (int row=0; row < alg::Mat4x4::Rows; ++row) {
        for (int col=0; col < alg::Mat4x4::Cols; ++col) {
            EXPECT_NEAR(algMat(row, col), glmMat[row][col], 0.001);
        }
    }
}


TEST(QuaternionTests, NormalizationComparison) {
    alg::Quat algQuat(1.0, 2.0, 3.0, 4.0);
    glm::quat glmQuat(4.0, 1.0, 2.0, 3.0);

    algQuat = algQuat.Normalize();
    glmQuat = glm::normalize(glmQuat);

    ASSERT_FLOAT_EQ(algQuat.X(), glmQuat.x);
    ASSERT_FLOAT_EQ(algQuat.Y(), glmQuat.y);
    ASSERT_FLOAT_EQ(algQuat.Z(), glmQuat.z);
    ASSERT_FLOAT_EQ(algQuat.W(), glmQuat.w);
}


TEST(QuaternionTests, EulerAngles) {
    alg::Quat algQuat(1.0, 2.0, 3.0, 4.0);
    glm::quat glmQuat(4.0, 1.0, 2.0, 3.0);

    auto algEuler = algQuat.ToRollPitchYaw();
    auto glmEuler = glm::eulerAngles(glmQuat);

    EXPECT_FLOAT_EQ(algEuler.X(), glmEuler.x);
    EXPECT_FLOAT_EQ(algEuler.Y(), glmEuler.y);
    EXPECT_FLOAT_EQ(algEuler.Z(), glmEuler.z);
}
