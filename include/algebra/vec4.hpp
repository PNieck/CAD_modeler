#pragma once

#include "vec3.hpp"

#include <array>

#include <ostream>


namespace alg
{
    template <typename DataType>
    class Vector4 {
    public:
        static constexpr size_t dim = 4;

        Vector4() = default;

        explicit Vector4(DataType scalar) { data.fill(scalar); }

        constexpr Vector4(DataType x, DataType y, DataType z, DataType w) {
            data[0] = x;
            data[1] = y;
            data[2] = z;
            data[3] = w;
        }

        Vector4(Vector3<DataType> v, DataType w):
            Vector4(v.X(), v.Y(), v.Z(), w) {}

        DataType& X()
            { return data[0]; }

        DataType X() const
            { return data[0]; }

        DataType& Y()
            { return data[1]; }

        DataType Y() const
            { return data[1]; }

        DataType& Z()
            { return data[2]; }

        DataType Z() const
            { return data[2]; }

        DataType& W()
            { return data[3]; }

        DataType W() const
            { return data[3]; }

        Vector4 Normalize() const {
            float len = Length();

            return Vector4(
                data[0] / len,
                data[1] / len,
                data[2] / len,
                data[3] / len
            );
        }

        [[nodiscard]]
        float LengthSquared() const
            { return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] + data[3]*data[3]; }
        
        [[nodiscard]]
        float Length() const
            { return std::sqrt(LengthSquared()); }
        

        DataType* Data()
            { return data.data(); }

        const DataType* Data() const
            { return data.data(); }

        DataType& operator[](const size_t pos)
            { return data[pos]; }

        const DataType& operator[](const size_t pos) const
            { return data[pos]; }

        Vector4 operator+(const Vector4& v) const {
            return Vector4(
                data[0] + v.data[0],
                data[1] + v.data[1],
                data[2] + v.data[2],
                data[3] + v.data[3]
            );
        }

        Vector4 operator-(const Vector4& v) const {
            return Vector4(
                data[0] - v.data[0],
                data[1] - v.data[1],
                data[2] - v.data[2],
                data[3] - v.data[3]
            );
        }

        Vector4 operator-() const {
            return Vector4(
                -data[0],
                -data[1],
                -data[2],
                -data[3]
            );
        }

    protected:
        std::array<DataType, 4> data;
    };


    using Vec4 = Vector4<float>;


    template <typename DataType>
    bool operator==(const Vector4<DataType>& v1, const Vector4<DataType>& v2) {
        return v1.X() == v2.X() &&
               v1.Y() == v2.Y() &&
               v1.Z() == v2.Z() &&
               v1.W() == v2.W();
    }


    template <typename DataType>
    Vector4<DataType> operator*(DataType scalar, const Vector4<DataType>& v) {
        return Vector4<DataType>(
            v.X() * scalar,
            v.Y() * scalar,
            v.Z() * scalar,
            v.W() * scalar
        );
    }


    template <typename DataType>
    DataType DistanceSquared(const Vector4<DataType>& v1, const Vector4<DataType>& v2) {
        return (v1 - v2).LengthSquared();
    }


    template <typename DataType>
    std::ostream& operator<< (std::ostream& stream, const Vector4<DataType>& vec) {
        stream << "[ " << vec.X() << ", " << vec.Y() << ", " << vec.Z() << ", " << vec.W() << " ]";

        return stream;
    }


    template <typename DataType>
    bool IsAnyNaN(const Vector4<DataType>& vec) {
        return std::isnan(vec.X()) || std::isnan(vec.Y()) || std::isnan(vec.Z()) || std::isnan(vec.W());
    }
}
