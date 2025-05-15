#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <ostream>


namespace alg
{
    template <typename DataType>
    class Vector2 {
    public:
        static constexpr std::size_t dim = 2;

        Vector2() = default;
        explicit constexpr Vector2(DataType scalar) { data.fill(scalar); }
        constexpr Vector2(DataType v1, DataType v2)
            { data[0] = v1; data[1] = v2; }


        DataType& X()
            { return data[0]; }

        DataType X() const
            { return data[0]; }

        DataType& Y()
            { return data[1]; }

        DataType Y() const
            { return data[1]; }
        

        DataType* Data()
            { return data.data(); }

        const DataType* Data() const
            { return data.data(); }

        Vector2 operator+(const Vector2& v) const {
            return Vector2(
                data[0] + v.data[0],
                data[1] + v.data[1]
            );
        }

        Vector2 operator-(const Vector2& v) const {
            return Vector2(
                data[0] - v.data[0],
                data[1] - v.data[1]
            );
        }

        [[nodiscard]]
        float LengthSquared() const
            { return data[0]*data[0] + data[1]*data[1]; }

        [[nodiscard]]
        float Length() const
            { return std::sqrt(LengthSquared()); }

    protected:
        std::array<DataType, 2> data;
    };


    using IVec2 = Vector2<int>;
    using Vec2 = Vector2<float>;


    template <typename DataType>
    DataType Distance(const Vector2<DataType>& v1, const Vector2<DataType>& v2) {
        return (v1 - v2).Length();
    }

    template <typename DataType>
    DataType DistanceSquared(const Vector2<DataType>& v1, const Vector2<DataType>& v2) {
        return (v1 - v2).LengthSquared();
    }

    template <typename DataType>
    bool operator==(const Vector2<DataType>& v1, const Vector2<DataType>& v2) {
        return v1.X() == v2.X() &&
               v1.Y() == v2.Y();
    }

    template <typename DataType>
    std::ostream& operator<< (std::ostream& stream, const Vector2<DataType>& vec) {
        stream << "[ " << vec.X() << ", " << vec.Y() << " ]";

        return stream;
    }
}
