#pragma once

#include <array>
#include <optional>

#include "vec4.hpp"


namespace alg
{
    class Mat4x4 {
    public:
        Mat4x4() = default;

        Mat4x4(
            float _00, float _01, float _02, float _03,
            float _10, float _11, float _12, float _13,
            float _20, float _21, float _22, float _23,
            float _30, float _31, float _32, float _33
        );

        inline float& operator()(const size_t row, const size_t col)
            { return data[row * Rows + col]; }

        inline float operator()(const size_t row, const size_t col) const
            { return data[row * Rows + col]; }

        [[nodiscard]]
        std::optional<Mat4x4> Inverse() const;

        void TransposeSelf();

        inline float* Data()
            { return data.data(); }

        [[nodiscard]]
        const float* Data() const
            { return data.data(); }

        static constexpr int Rows = 4;
        static constexpr int Cols = 4;

        Mat4x4& operator*=(float scalar);

        friend Vec4 operator*(const Mat4x4& mat, const Vec4& vec);
        friend Vec4 operator*(const Vec4& vec, const Mat4x4& mat);
        friend Mat4x4 operator*(const Mat4x4& mat1, const Mat4x4& mat2);

    private:
        std::array<float, Rows*Cols> data;
    };


    Mat4x4 TranslationMtx(const Vec3& vector);


    Mat4x4 TranslationInverseMtx(const Vec3& vector);


    Mat4x4 LookAt(const Vec3& cameraPos, const Vec3& direction, const Vec3& upVector);


    Mat4x4 Frustum(float near, float far, float left, float right, float top, float bottom);
}
