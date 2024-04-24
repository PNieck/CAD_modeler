#include <gtest/gtest.h>

#include <algebra/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>


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
}
