#pragma once

#include <vector>
#include <ranges>


class BitSet2D {
public:
    BitSet2D(const size_t rows, const size_t cols):
        data(rows * cols), rows(rows), cols(cols) {}

    BitSet2D(const size_t rows, const size_t cols, const bool val):
        data(rows * cols, val), rows(rows), cols(cols) {}

    [[nodiscard]]
    bool At(const size_t row, const size_t col) const
    { return data.at(row*cols + col); }

    void Set(const size_t row, const size_t col, const bool val)
        { data.at(row*cols + col) = val; }

    [[nodiscard]]
    size_t Rows() const
        { return rows; }

    [[nodiscard]]
    size_t Cols() const
        { return cols; }

    [[nodiscard]]
    size_t Len() const
        { return rows * cols; }

    // Iterators
    [[nodiscard]]
    auto begin()
        { return data.begin(); }

    [[nodiscard]]
    auto end()
        { return data.end(); }

private:
    std::vector<bool> data;

    size_t rows, cols;
};
