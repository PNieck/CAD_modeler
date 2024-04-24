#include <gtest/gtest.h>

#include <algebra/mat4x4.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>


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
