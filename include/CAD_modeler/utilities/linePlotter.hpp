#pragma once

#include <algebra/vec2.hpp>

#include <deque>

class LinePlotter {
public:
    static std::deque<alg::IVec2> DeterminePlotLines(int x0, int y0, int x1, int y1, int thickness);

private:
    static std::deque<alg::IVec2> PlotLineLow(int x0, int y0, int x1, int y1, int thickness);
    static std::deque<alg::IVec2> PlotLineHigh(int x0, int y0, int x1, int y1, int thickness);
};
