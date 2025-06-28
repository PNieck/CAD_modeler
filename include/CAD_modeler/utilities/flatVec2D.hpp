#pragma once

#include <vector>


template <typename T>
class FlatVec2D {
public:
    FlatVec2D(const size_t rows, const size_t cols):
        data(rows * cols), rows(rows), cols(cols) {}

    FlatVec2D(const size_t rows, const size_t cols, const T& val):
        data(rows * cols, val), rows(rows), cols(cols) {}

    T& At(const size_t row, const size_t col)
        { return data.at(row*cols + col); }

    const T& At(const size_t row, const size_t col) const
        { return data.at(row*cols + col); }

    [[nodiscard]]
    unsigned int Rows() const
        { return rows; }

    [[nodiscard]]
    unsigned int Cols() const
        { return cols; }

    T* Data()
        { return data.data(); }

    const T* Data() const
        { return data.data(); }

private:
    std::vector<T> data;

    size_t rows, cols;
};
