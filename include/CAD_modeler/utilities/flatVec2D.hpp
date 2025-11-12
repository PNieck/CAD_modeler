#pragma once

#include <vector>
#include <ranges>


template <typename T>
class FlatVec2D {
public:
    FlatVec2D(const size_t rows, const size_t cols):
        data(rows * cols), rows(rows), cols(cols) {}

    FlatVec2D(const size_t rows, const size_t cols, const T& val):
        data(rows * cols, val), rows(rows), cols(cols) {}

    [[nodiscard]]
    T& At(const size_t row, const size_t col)
        { return data.at(row*cols + col); }

    [[nodiscard]]
    const T& At(const size_t row, const size_t col) const
        { return data.at(row*cols + col); }

    [[nodiscard]]
    size_t Rows() const
        { return rows; }

    [[nodiscard]]
    size_t Cols() const
        { return cols; }

    [[nodiscard]]
    size_t Len() const
        { return rows * cols; }

    [[nodiscard]]
    T* Data()
        { return data.data(); }

    [[nodiscard]]
    const T* Data() const
        { return data.data(); }

    // Iterators
    [[nodiscard]]
    auto begin()
        { return data.begin(); }

    [[nodiscard]]
    auto end()
        { return data.end(); }

private:
    std::vector<T> data;

    size_t rows, cols;
};


static_assert(std::ranges::contiguous_range<FlatVec2D<int>>);
