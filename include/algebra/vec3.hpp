#pragma once

#include <array>
#include <cmath>
#include <ostream>


namespace alg
{
    template <typename DataType>
    class Vector3 {
    public:
        static constexpr size_t dim = 3;

        Vector3() = default;
        explicit constexpr Vector3(DataType scalar) { data.fill(scalar); }
        constexpr Vector3(DataType v1, DataType v2, DataType v3) {
            data[0] = v1;
            data[1] = v2;
            data[2] = v3;
        }

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

        Vector3 Normalize() const {
            float len = Length();

            return Vector3(
                data[0] / len,
                data[1] / len,
                data[2] / len
            );
        }

        [[nodiscard]]
        float LengthSquared() const
            { return data[0]*data[0] + data[1]*data[1] + data[2]*data[2]; }
        
        [[nodiscard]]
        float Length() const
            { return std::sqrt(LengthSquared()); }

        DataType* Data()
            { return data.data(); }

        const DataType* Data() const
            { return data.data(); }

        Vector3<DataType> operator+(const Vector3<DataType>& v) const {
            return Vector3<DataType>(
        Vector3 operator+(const Vector3& v) const {
            return Vector3(
                data[0] + v.data[0],
                data[1] + v.data[1],
                data[2] + v.data[2]
            );
        }

        Vector3& operator+=(const Vector3& v) {
            data[0] += v.data[0];
            data[1] += v.data[1];
            data[2] += v.data[2];
            
            return *this;
        }

        Vector3& operator-=(const Vector3& v) {
            data[0] -= v.data[0];
            data[1] -= v.data[1];
            data[2] -= v.data[2];
            
            return *this;
        }

        Vector3& operator*=(DataType scalar) {
            data[0] *= scalar;
            data[1] *= scalar;
            data[2] *= scalar;
            
            return *this;
        }

        Vector3 operator-(const Vector3& v) const {
            return Vector3(
                data[0] - v.data[0],
                data[1] - v.data[1],
                data[2] - v.data[2]
            );
        }


        Vector3 operator-() const {
            return Vector3(
                -data[0],
                -data[1],
                -data[2]
            );
        }


        Vector3& operator/=(DataType scalar) {
            data[0] /= scalar;
            data[1] /= scalar;
            data[2] /= scalar;
            
            return *this;
        }


        Vector3& operator=(const Vector3& v) {
            data[0] = v.data[0];
            data[1] = v.data[1];
            data[2] = v.data[2];

            return *this;
        }

    protected:
        std::array<DataType, 3> data;
    };


    using Vec3 = Vector3<float>;


    template <typename DataType>
    Vector3<DataType> Cross(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return Vector3<DataType>(
            v1.Y() * v2.Z() - v1.Z() * v2.Y(),
            v1.Z() * v2.X() - v1.X() * v2.Z(),
            v1.X() * v2.Y() - v1.Y() * v2.X()
        );
    }


    template <typename DataType>
    DataType Dot(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return v1.X() * v2.X() + v1.Y() * v2.Y() + v1.Z() * v2.Z();
    }


    template <typename DataType>
    DataType Distance(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return (v1 - v2).Length();
    }


    template <typename DataType>
    DataType DistanceSquared(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return (v1 - v2).LengthSquared();
    }


    template <typename DataType>
    Vector3<DataType> operator*(DataType scalar, const Vector3<DataType>& v) {
        return Vector3<DataType>(
            v.X() * scalar,
            v.Y() * scalar,
            v.Z() * scalar
        );
    }


    template <typename DataType>
    Vector3<DataType> operator*(const Vector3<DataType>& v, DataType scalar)
        { return scalar * v; }


    template <typename DataType>
    Vector3<DataType> operator/(const Vector3<DataType>& v, DataType scalar)
        { return v * (1.f / scalar); }


    template <typename DataType>
    bool operator==(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return v1.X() == v2.X() &&
               v1.Y() == v2.Y() &&
               v1.Z() == v2.Z();
    }


    template <typename DataType>
    bool operator!=(const Vector3<DataType>& v1, const Vector3<DataType>& v2) {
        return !(v1 == v2);
    }

    template <typename DataType>
    Vector3<DataType> GetPerpendicularVec(const Vector3<DataType>& v) {
        Vector3<DataType> cross = Cross(v, Vector3<DataType>(0, 0, 1));

        if (cross.LengthSquared() == 0.0f)
            cross = Cross(v, Vector3<DataType>(0, 1, 0));
        
        return cross.Normalize();
    }


    template <typename DataType>
    std::ostream& operator<< (std::ostream& stream, const Vector3<DataType>& vec) {
        stream << "[ " << vec.X() << ", " << vec.Y() << ", " << vec.Z() << " ]";

        return stream;
    }

}
