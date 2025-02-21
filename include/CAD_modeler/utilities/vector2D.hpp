#pragma once

#include <vector>
#include <algorithm>


template <typename t>
class Vector2D {
public:
    Vector2D(unsigned int rows, unsigned int cols);

    void AddRows(unsigned int cnt);
    inline void AddRow()
        { AddRows(1); }

    void AddCols(unsigned int cnt);
    inline void AddCol()
        { AddCols(1); }

    void DeleteRows(unsigned int cnt);
    inline void DeleteRow()
        { DeleteRows(1); }

    void DeleteCols(unsigned int cnt);
    inline void DeleteCol()
        { DeleteCols(1); }

    unsigned int Rows() const
        { return rows; }

    unsigned int Cols() const
        { return cols; }

    inline t& At(int row, int col)
        { return data.at(row).at(col); }

    inline const t& At(int row, int col) const
        { return data.at(row).at(col); }

private:
    unsigned int rows;
    unsigned int cols;

    std::vector<std::vector<t>> data;
};


template <typename t>
inline Vector2D<t>::Vector2D(unsigned int rows, unsigned int cols):
    rows(rows), cols(cols), data(rows)
{
    for (auto& row: data) {
        row.resize(cols);
    }
}


template <typename t>
inline void Vector2D<t>::AddRows(const unsigned int cnt)
{
    rows += cnt;

    data.resize(rows);

    for (unsigned int i = rows-cnt; i < rows; ++i) {
        data[i].resize(cols);
    }
}


template <typename t>
inline void Vector2D<t>::AddCols(const unsigned int cnt)
{
    cols += cnt;

    for (auto& row: data) {
        row.resize(cols);
    }
}


template <typename t>
inline void Vector2D<t>::DeleteRows(const unsigned int cnt)
{
    rows = std::max<unsigned int>(0u, rows - cnt);

    data.resize(rows);
}


template <typename t>
inline void Vector2D<t>::DeleteCols(const unsigned int cnt)
{
    cols = std::max<unsigned int>(0u, cols - cnt);

    for (auto& row: data) {
        row.resize(cols);
    }
}
