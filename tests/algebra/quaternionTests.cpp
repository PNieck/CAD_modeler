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
    glmQuat = normalize(glmQuat);

    auto algMat = algQuat.ToRotationMatrix();
    auto glmMat = toMat4(glmQuat);

    for (int row=0; row < alg::Mat4x4::Rows; ++row) {
        for (int col=0; col < alg::Mat4x4::Cols; ++col) {
            std::cout << algMat(row, col) << " ";
        }
        std::cout << "\n";
    }

    std::cout << to_string(glmMat);

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
    glmQuat = normalize(glmQuat);

    ASSERT_FLOAT_EQ(algQuat.X(), glmQuat.x);
    ASSERT_FLOAT_EQ(algQuat.Y(), glmQuat.y);
    ASSERT_FLOAT_EQ(algQuat.Z(), glmQuat.z);
    ASSERT_FLOAT_EQ(algQuat.W(), glmQuat.w);
}


TEST(QuaternionTests, EulerAngles) {
    const alg::Quat algQuat(1.0, 2.0, 3.0, 4.0);
    constexpr glm::quat glmQuat(4.0, 1.0, 2.0, 3.0);

    auto algEuler = algQuat.ToRollPitchYaw();
    const auto glmEuler = eulerAngles(glmQuat);

    EXPECT_FLOAT_EQ(algEuler.X(), glmEuler.x);
    EXPECT_FLOAT_EQ(algEuler.Y(), glmEuler.y);
    EXPECT_FLOAT_EQ(algEuler.Z(), glmEuler.z);
}


TEST(QuaternionTests, CheckingQuaternionBetweenVectors) {
    constexpr alg::Vec3 v1(1.f, 2.f, 3.f);
    alg::Vec3 v2(5.f, 6.0f, 7.f);

    const alg::Quat q = alg::Quat::FromVectors(v1, v2);
    const alg::Vec3 rotated = q.Rotate(v1);

    // Scaling v2 so it has the same length as v1
    v2 = v2.Normalize() * v1.Length();

    EXPECT_FLOAT_EQ(v2.X(), rotated.X());
    EXPECT_FLOAT_EQ(v2.Y(), rotated.Y());
    EXPECT_FLOAT_EQ(v2.Z(), rotated.Z());
}


TEST(QuaternionTests, IndentityQuaternionBetweenTheSameVectors) {
    constexpr alg::Vec3 v1(1.f, 2.f, 3.f);

    const alg::Quat q = alg::Quat::FromVectors(v1, v1);

    ASSERT_EQ(q.X(), alg::Quat::Identity().X());
    ASSERT_EQ(q.Y(), alg::Quat::Identity().Y());
    ASSERT_EQ(q.Z(), alg::Quat::Identity().Z());
    ASSERT_EQ(q.W(), alg::Quat::Identity().W());
}


TEST(QuaternionTests, QuaternionBetweenAntiparallelVectors) {
    constexpr alg::Vec3 v1(1.f, 2.f, 3.f);
    alg::Vec3 v2 = -v1;

    const alg::Quat q = alg::Quat::FromVectors(v1, v2);
    const alg::Vec3 rotated = q.Rotate(v1);

    EXPECT_FLOAT_EQ(v2.X(), rotated.X());
    EXPECT_FLOAT_EQ(v2.Y(), rotated.Y());
    EXPECT_FLOAT_EQ(v2.Z(), rotated.Z());
}
