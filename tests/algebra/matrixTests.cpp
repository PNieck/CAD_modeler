#include <gtest/gtest.h>

#include <algebra/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


TEST(MatrixTests, MatrixDataIsCorrectForOpenGL) {
    alg::Mat4x4 algMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    glm::mat4x4 glmMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    float* algMatPtr = algMat.Data();
    float* glmMatPtr = glm::value_ptr(glmMat);

    constexpr int elementsCnt = alg::Mat4x4::Rows * alg::Mat4x4::Cols;
    for (int i=0; i < elementsCnt; i++) {
        ASSERT_FLOAT_EQ(*(algMatPtr + i), *(glmMatPtr + i));
    }
}


TEST(MatrixTests, GettingElementsComparison) {
    alg::Mat4x4 algMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    glm::mat4x4 glmMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    for (int row=0; row < alg::Mat4x4::Rows; ++row) {
        for (int col=0; col < alg::Mat4x4::Cols; ++col) {
            ASSERT_EQ(algMat(row, col), glmMat[row][col]);
        }
    }
}


TEST(MatrixTests, MultiplicationWithVectorComparison) {
    alg::Mat4x4 algMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    alg::Vec4 algV(1.0, 2.0, 3.0, 4.0);

    glm::mat4x4 glmMat(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    glm::vec4 glmV(1.0, 2.0, 3.0, 4.0);

    auto resultAlg = algMat * algV;
    auto resultGlm = glmMat * glmV;

    EXPECT_EQ(resultAlg.X(), resultGlm.x);
    EXPECT_EQ(resultAlg.Y(), resultGlm.y);
    EXPECT_EQ(resultAlg.Z(), resultGlm.z);
    EXPECT_EQ(resultAlg.W(), resultGlm.w);

    resultAlg =  algV * algMat;
    resultGlm =  glmV * glmMat;

    EXPECT_EQ(resultAlg.X(), resultGlm.x);
    EXPECT_EQ(resultAlg.Y(), resultGlm.y);
    EXPECT_EQ(resultAlg.Z(), resultGlm.z);
    EXPECT_EQ(resultAlg.W(), resultGlm.w);
}


TEST(MatrixTests, MatrixMultiplicationComparison) {
    alg::Mat4x4 algMat1(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    alg::Mat4x4 algMat2(
        17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0,
        25.0, 26.0, 27.0, 28.0,
        29.0, 30.0, 31.0, 32.0
    );

    auto resultAlg = algMat1 * algMat2;

    glm::mat4x4 glmMat1(
         1.0,  2.0,  3.0,  4.0,
         5.0,  6.0,  7.0,  8.0,
         9.0, 10.0, 11.0, 12.0,
        13.0, 14.0, 15.0, 16.0
    );

    glm::mat4x4 glmMat2(
        17.0, 18.0, 19.0, 20.0,
        21.0, 22.0, 23.0, 24.0,
        25.0, 26.0, 27.0, 28.0,
        29.0, 30.0, 31.0, 32.0
    );

    auto resultGlm = glmMat1 * glmMat2;

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            ASSERT_EQ(resultAlg(row, col), resultGlm[row][col]);
        }
    }

    resultAlg = algMat2 * algMat1;
    resultGlm = glmMat2 * glmMat1;

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            ASSERT_EQ(resultAlg(row, col), resultGlm[row][col]);
        }
    }
}


TEST(MatrixTests, MatrixInversion) {
    alg::Mat4x4 mat(
        2.f, 5.f, 0.f, 8.f,
        1.f, 4.f, 2.f, 6.f,
        7.f, 8.f, 9.f, 3.f,
        1.f, 5.f, 7.f, 8.f
    );

    auto inv = mat.Inverse();

    ASSERT_TRUE(inv.has_value());

    auto identity = inv.value() * mat;

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            EXPECT_NEAR(identity(row, col), row==col ? 1.f : 0.f, 0.001);
        }
    }

    identity = mat * inv.value();

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            EXPECT_NEAR(identity(row, col), row==col ? 1.f : 0.f, 0.001);
        }
    }
}


TEST(MatrixTests, FrustumMatrixComparison)
{
    float near = 0.1f;
    float far = 100.0f;
    float top = 5.0f;
    float bottom = -2.0f;
    float right = 2.0f;
    float left = -4.0f;

    auto algMtx = alg::Frustum(near, far, left, right, top, bottom);
    auto glmMtx = glm::frustum(left, right, bottom, top, near, far);

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            ASSERT_EQ(algMtx(row, col), glmMtx[row][col]);
        }
    }
}


TEST(MatrixTests, OrthographicsMatrixComparison)
{
    constexpr float left = -5.f;
    constexpr float right = 7.f;
    constexpr float top = 8.f;
    constexpr float bottom = -3.f;
    constexpr float near = 1.f;
    constexpr float far = 10.f;

    auto algMtx = alg::OrthographicProjection(left, right, top, bottom, near, far);
    auto glmMtx = glm::ortho(left, right, bottom, top, near, far);

    for (int row=0; row < 4; row++) {
        for (int col=0; col < 4; col++) {
            ASSERT_EQ(algMtx(row, col), glmMtx[row][col]);
        }
    }
}
