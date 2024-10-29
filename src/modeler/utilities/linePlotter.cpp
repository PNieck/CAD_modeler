#include <CAD_modeler/utilities/linePlotter.hpp>


std::deque<alg::IVec2> LinePlotter::DeterminePlotLines(int x0, int y0, int x1, int y1, int thickness)
{
    if (std::abs(y1 - y0) < std::abs(x1 - x0)) {
        if (x0 > x1)
            return PlotLineLow(x1, y1, x0, y0, thickness);
        else
            return PlotLineLow(x0, y0, x1, y1, thickness);
    }
    else {
        if (y0 > y1)
            return PlotLineHigh(x1, y1, x0, y0, thickness);
        else
            return PlotLineHigh(x0, y0, x1, y1, thickness);
    }
}


std::deque<alg::IVec2> LinePlotter::PlotLineLow(int x0, int y0, int x1, int y1, int thickness)
{
    std::deque<alg::IVec2> result;

    const int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    int D = 2*dy - dx;
    int y = y0;

    for (int x = x0; x <= x1; x++) {
        for (int i= -thickness/2-1; i < thickness/2+1; i++) {
            result.emplace_back(x, y+i);
        }


        if (D > 0) {
            y += yi;
            D += 2*(dy - dx);
        }
        else
            D += 2*dy;
    }

    return result;
}


std::deque<alg::IVec2> LinePlotter::PlotLineHigh(int x0, int y0, int x1, int y1, int thickness)
{
    std::deque<alg::IVec2> result;

    int dx = x1 - x0;
    const int dy = y1 - y0;
    int xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    int D = (2*dx) - dy;
    int x = x0;

    for (int y = y0; y <= y1; y++) {
        for (int i= -thickness/2-1; i < thickness/2+1; i++) {
            result.emplace_back(x+i, y);
        }

        if (D > 0) {
            x += xi;
            D += 2*(dx - dy);
        }
        else
            D += 2*dx;
    }

    return result;
}
