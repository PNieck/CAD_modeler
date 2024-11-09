#pragma once

#include <array>


namespace alg
{
    template <typename DataType>
    class Vector2 {
    public:
        static constexpr size_t dim = 2;

        Vector2() = default;
        explicit Vector2(DataType scalar) { data.fill(scalar); }
        Vector2(DataType v1, DataType v2)
            { data[0] = v1; data[1] = v2; }


        inline DataType& X()
            { return data[0]; }

        inline DataType X() const
            { return data[0]; }

        inline DataType& Y()
            { return data[1]; }

        inline DataType Y() const
            { return data[1]; }
        

        inline DataType* Data()
            { return data.data(); }

        inline const DataType* Data() const
            { return data.data(); }

        Vector2<DataType> operator+(const Vector2<DataType>& v) const {
            return Vector2<DataType>(
                data[0] + v.data[0],
                data[1] + v.data[1]
            );
        }

        Vector2<DataType> operator-(const Vector2<DataType>& v) const {
            return Vector2<DataType>(
                data[0] - v.data[0],
                data[1] - v.data[1]
            );
        }

    protected:
        std::array<DataType, 2> data;
    };


    using IVec2 = Vector2<int>;
}
