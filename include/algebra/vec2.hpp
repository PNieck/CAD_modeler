#pragma once

#include <array>
#include <cstddef>


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

    protected:
        std::array<DataType, 2> data;
    };


    using IVec2 = Vector2<int>;
}
