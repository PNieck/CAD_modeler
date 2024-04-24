#pragma once

#include "vec3.hpp"

#include <array>


namespace alg
{
    template <typename DataType>
    class Vector4 {
    public:
        Vector4() = default;

        Vector4(DataType scalar) { data.fill(scalar); }

        constexpr Vector4(DataType x, DataType y, DataType z, DataType w) {
            data[0] = x;
            data[1] = y;
            data[2] = z;
            data[3] = w;
        }

        Vector4(Vector3<DataType> v, DataType w):
            Vector4(v.X(), v.Y(), v.Z(), w) {}

        inline DataType& X()
            { return data[0]; }

        inline DataType X() const
            { return data[0]; }

        inline DataType& Y()
            { return data[1]; }

        inline DataType Y() const
            { return data[1]; }

        inline DataType& Z()
            { return data[2]; }

        inline DataType Z() const
            { return data[2]; }

        inline DataType& W()
            { return data[3]; }

        inline DataType W() const
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

        inline float LengthSquared() const
            { return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] * data[3]*data[3]; }
        
        inline float Length() const
            { return std::sqrt(LengthSquared()); }
        

        inline DataType* Data()
            { return data.data(); }

        inline const DataType* Data() const
            { return data.data(); }

        Vector4<DataType> operator+(const Vector4<DataType>& v) const {
            return Vector4<DataType>(
                data[0] + v.data[0],
                data[1] + v.data[1],
                data[2] + v.data[2],
                data[3] + v.data[3]
            );
        }

        Vector4<DataType> operator-(const Vector4<DataType>& v) const {
            return Vector4<DataType>(
                data[0] - v.data[0],
                data[1] - v.data[1],
                data[2] - v.data[2],
                data[3] - v.data[3]
            );
        }

    protected:
        std::array<DataType, 4> data;
    };


    using Vec4 = Vector4<float>;
};
