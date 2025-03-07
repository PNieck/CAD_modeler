#pragma once

#include <algebra/vec2.hpp>

#include <deque>
#include <tuple>

class LinePlotter {
public:
    enum class LineType {
        High,
        Low
    };

    static std::tuple<std::deque<alg::IVec2>, LineType> DeterminePlotLines(int x0, int y0, int x1, int y1);

private:
    static std::deque<alg::IVec2> PlotLineLow(int x0, int y0, int x1, int y1);
    static std::deque<alg::IVec2> PlotLineHigh(int x0, int y0, int x1, int y1);
};
