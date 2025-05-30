#include <algebra/mat4x4.hpp>

#include <cassert>


alg::Mat4x4::Mat4x4(
    const float _00, const float _01, const float _02, const float _03,
    const float _10, const float _11, const float _12, const float _13,
    const float _20, const float _21, const float _22, const float _23,
    const float _30, const float _31, const float _32, const float _33
) {
     data[0] = _00;   data[1] = _01;   data[2] = _02;   data[3] = _03;
     data[4] = _10;   data[5] = _11;   data[6] = _12;   data[7] = _13;
     data[8] = _20;   data[9] = _21;  data[10] = _22;  data[11] = _23;
    data[12] = _30;  data[13] = _31;  data[14] = _32;  data[15] = _33;
}


std::optional<alg::Mat4x4> alg::Mat4x4::Inverse() const
{
    Mat4x4 result{};

    result.data[0] = data[5]  * data[10] * data[15] -
                     data[5]  * data[11] * data[14] -
                     data[9]  * data[6]  * data[15] +
                     data[9]  * data[7]  * data[14] +
                     data[13] * data[6]  * data[11] -
                     data[13] * data[7]  * data[10];

    result.data[4] = -data[4]  * data[10] * data[15] +
                      data[4]  * data[11] * data[14] +
                      data[8]  * data[6]  * data[15] -
                      data[8]  * data[7]  * data[14] -
                      data[12] * data[6]  * data[11] +
                      data[12] * data[7]  * data[10];

    result.data[8] = data[4]  * data[9]  * data[15] -
                     data[4]  * data[11] * data[13] -
                     data[8]  * data[5]  * data[15] +
                     data[8]  * data[7]  * data[13] +
                     data[12] * data[5]  * data[11] -
                     data[12] * data[7]  * data[9];

    result.data[12] = -data[4]  * data[9]  * data[14] +
                       data[4]  * data[10] * data[13] +
                       data[8]  * data[5]  * data[14] -
                       data[8]  * data[6]  * data[13] -
                       data[12] * data[5]  * data[10] +
                       data[12] * data[6]  * data[9];

    const float det = data[0] * result.data[0] + data[1] * result.data[4] + data[2] * result.data[8] + data[3] * result.data[12];

    // Checking if matrix can be inverted
    if (det == 0)
        return {};

    result.data[1] = -data[1]  * data[10] * data[15] + 
                      data[1]  * data[11] * data[14] + 
                      data[9]  * data[2] * data[15] - 
                      data[9]  * data[3] * data[14] - 
                      data[13] * data[2] * data[11] + 
                      data[13] * data[3] * data[10];

    result.data[5] = data[0]  * data[10] * data[15] - 
                     data[0]  * data[11] * data[14] - 
                     data[8]  * data[2] * data[15] + 
                     data[8]  * data[3] * data[14] + 
                     data[12] * data[2] * data[11] - 
                     data[12] * data[3] * data[10];

    result.data[9] = -data[0]  * data[9] * data[15] + 
                      data[0]  * data[11] * data[13] + 
                      data[8]  * data[1] * data[15] - 
                      data[8]  * data[3] * data[13] - 
                      data[12] * data[1] * data[11] + 
                      data[12] * data[3] * data[9];

    result.data[13] = data[0]  * data[9] * data[14] - 
                      data[0]  * data[10] * data[13] - 
                      data[8]  * data[1] * data[14] + 
                      data[8]  * data[2] * data[13] + 
                      data[12] * data[1] * data[10] - 
                      data[12] * data[2] * data[9];

    result.data[2] = data[1]  * data[6] * data[15] - 
                     data[1]  * data[7] * data[14] - 
                     data[5]  * data[2] * data[15] + 
                     data[5]  * data[3] * data[14] + 
                     data[13] * data[2] * data[7] - 
                     data[13] * data[3] * data[6];

    result.data[6] = -data[0]  * data[6] * data[15] + 
                      data[0]  * data[7] * data[14] + 
                      data[4]  * data[2] * data[15] - 
                      data[4]  * data[3] * data[14] - 
                      data[12] * data[2] * data[7] + 
                      data[12] * data[3] * data[6];

    result.data[10] = data[0]  * data[5] * data[15] - 
                      data[0]  * data[7] * data[13] - 
                      data[4]  * data[1] * data[15] + 
                      data[4]  * data[3] * data[13] + 
                      data[12] * data[1] * data[7] - 
                      data[12] * data[3] * data[5];

    result.data[14] = -data[0]  * data[5] * data[14] + 
                       data[0]  * data[6] * data[13] + 
                       data[4]  * data[1] * data[14] - 
                       data[4]  * data[2] * data[13] - 
                       data[12] * data[1] * data[6] + 
                       data[12] * data[2] * data[5];

    result.data[3] = -data[1] * data[6] * data[11] + 
                      data[1] * data[7] * data[10] + 
                      data[5] * data[2] * data[11] - 
                      data[5] * data[3] * data[10] - 
                      data[9] * data[2] * data[7] + 
                      data[9] * data[3] * data[6];

    result.data[7] = data[0] * data[6] * data[11] - 
                     data[0] * data[7] * data[10] - 
                     data[4] * data[2] * data[11] + 
                     data[4] * data[3] * data[10] + 
                     data[8] * data[2] * data[7] - 
                     data[8] * data[3] * data[6];

    result.data[11] = -data[0] * data[5] * data[11] + 
                       data[0] * data[7] * data[9] + 
                       data[4] * data[1] * data[11] - 
                       data[4] * data[3] * data[9] - 
                       data[8] * data[1] * data[7] + 
                       data[8] * data[3] * data[5];

    result.data[15] = data[0] * data[5] * data[10] - 
                      data[0] * data[6] * data[9] - 
                      data[4] * data[1] * data[10] + 
                      data[4] * data[2] * data[9] + 
                      data[8] * data[1] * data[6] - 
                      data[8] * data[2] * data[5];

    result *= 1.0f / det;

    return result;
}


void alg::Mat4x4::TransposeSelf()
{
    std::swap(data[1], data[4]);
    std::swap(data[2], data[8]);
    std::swap(data[3], data[12]);
    std::swap(data[6], data[9]);
    std::swap(data[7], data[13]);
    std::swap(data[11], data[14]);
}


alg::Mat4x4 & alg::Mat4x4::operator*=(const float scalar)
{
    for (auto& nb: data)
        nb *= scalar;

    return *this;
}


alg::Vec4 alg::operator*(const Mat4x4& mat, const Vec4& vec)
{
    return {
        mat(0,0) * vec.X() + mat(1, 0) * vec.Y() + mat(2, 0) * vec.Z() + mat(3,0) * vec.W(),
        mat(0,1) * vec.X() + mat(1, 1) * vec.Y() + mat(2, 1) * vec.Z() + mat(3,1) * vec.W(),
        mat(0,2) * vec.X() + mat(1, 2) * vec.Y() + mat(2, 2) * vec.Z() + mat(3,2) * vec.W(),
        mat(0,3) * vec.X() + mat(1, 3) * vec.Y() + mat(2, 3) * vec.Z() + mat(3,3) * vec.W()
    };
}


alg::Vec4 alg::operator*(const Vec4 & vec, const Mat4x4 & mat)
{
    return {
        mat(0, 0) * vec.X() + mat(0, 1) * vec.Y() + mat(0, 2) * vec.Z() + mat(0, 3) * vec.W(),
        mat(1, 0) * vec.X() + mat(1, 1) * vec.Y() + mat(1, 2) * vec.Z() + mat(1, 3) * vec.W(),
        mat(2, 0) * vec.X() + mat(2, 1) * vec.Y() + mat(2, 2) * vec.Z() + mat(2, 3) * vec.W(),
        mat(3, 0) * vec.X() + mat(3, 1) * vec.Y() + mat(3, 2) * vec.Z() + mat(3, 3) * vec.W()
    };
}


alg::Mat4x4 alg::operator*(const Mat4x4 & mat1, const Mat4x4 & mat2)
{
    Mat4x4 result{};

    for (int i=0; i < Mat4x4::Rows; ++i) {
        for (int j=0; j < Mat4x4::Cols; ++j) {
            result(i, j) = 0;

            for (int k=0; k < Mat4x4::Cols; ++k) {
                result(i, j) += mat2(i, k) * mat1(k, j);
            }
        }
    }

    return result;
}


alg::Mat4x4 alg::TranslationMtx(const Vec3 &vec)
{
    return {
           1.0f,    0.0f,    0.0f, 0.0f,
           0.0f,    1.0f,    0.0f, 0.0f,
           0.0f,    0.0f,    1.0f, 0.0f,
        vec.X(), vec.Y(), vec.Z(), 1.0f
    };
}


alg::Mat4x4 alg::TranslationInverseMtx(const Vec3& vec)
{
    return {
         1.0f,     0.0f,     0.0f, 0.0f,
         0.0f,     1.0f,     0.0f, 0.0f,
         0.0f,     0.0f,     1.0f, 0.0f,
     -vec.X(), -vec.Y(), -vec.Z(), 1.0f
    };
}


alg::Mat4x4 alg::LookAt(const Vec3 &cameraPos, const Vec3 &direction, const Vec3 &upVector)
{
    assert(direction.LengthSquared() != 0.f);
    assert(upVector.LengthSquared() != 0.f);

    Vec3 right = Cross(upVector, direction).Normalize();
    Vec3 up = Cross(direction, right).Normalize();

    const Mat4x4 translation = TranslationMtx(-cameraPos);

    // Matrix which translates canonical coordinate system to camera one
    const Mat4x4 coordTransform(
        right.X(), up.X(), direction.X(), 0.f,
        right.Y(), up.Y(), direction.Y(), 0.f,
        right.Z(), up.Z(), direction.Z(), 0.f,
              0.f,    0.f,     0.f, 1.f
    );

    return coordTransform * translation;
}


alg::Mat4x4 alg::Frustum(
    const float near, const float far, const float left, const float right, const float top, const float bottom
) {
    const float width = right - left;
    const float height = top - bottom;
    const float depth = far - near;

    Mat4x4 result(
        2.f*near/width, 0.f, (right + left)/width, 0.f,
        0.f, 2*near/height, (top+bottom)/height, 0.f,
        0.f, 0.f, -(far+near)/depth, -2*far*near/depth,
        0.f, 0.f, -1.f, 0.f
    );

    result.TransposeSelf();

    return result;
}
