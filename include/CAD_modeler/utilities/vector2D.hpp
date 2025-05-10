#pragma once

#include <vector>
#include <algorithm>


template <typename T>
class Vector2D {
public:
    Vector2D(unsigned int rows, unsigned int cols);

    void AddRows(unsigned int cnt);
    void AddRow()
        { AddRows(1); }

    void AddCols(unsigned int cnt);
    void AddCol()
        { AddCols(1); }

    void DeleteRows(unsigned int cnt);
    void DeleteRow()
        { DeleteRows(1); }

    void DeleteCols(unsigned int cnt);
    void DeleteCol()
        { DeleteCols(1); }

    [[nodiscard]]
    unsigned int Rows() const
        { return rows; }

    [[nodiscard]]
    unsigned int Cols() const
        { return cols; }

    T& At(int row, int col)
        { return data.at(row).at(col); }

    const T& At(int row, int col) const
        { return data.at(row).at(col); }

private:
    unsigned int rows;
    unsigned int cols;

    std::vector<std::vector<T>> data;
};


template <typename T>
Vector2D<T>::Vector2D(unsigned int rows, unsigned int cols):
    rows(rows), cols(cols), data(rows)
{
    for (auto& row: data) {
        row.resize(cols);
    }
}


template <typename T>
void Vector2D<T>::AddRows(const unsigned int cnt)
{
    rows += cnt;

    data.resize(rows);

    for (unsigned int i = rows-cnt; i < rows; ++i) {
        data[i].resize(cols);
    }
}


template <typename T>
void Vector2D<T>::AddCols(const unsigned int cnt)
{
    cols += cnt;

    for (auto& row: data) {
        row.resize(cols);
    }
}


template <typename T>
void Vector2D<T>::DeleteRows(const unsigned int cnt)
{
    rows = std::max<unsigned int>(0u, rows - cnt);

    data.resize(rows);
}


template <typename T>
void Vector2D<T>::DeleteCols(const unsigned int cnt)
{
    cols = std::max<unsigned int>(0u, cols - cnt);

    for (auto& row: data) {
        row.resize(cols);
    }
}
