#include <optimization/lineSearchMethods/dichotomyLineSearch.hpp>


float opt::DichotomyLineSearch::Search(FunctionToOptimize &fun, const std::vector<float>& start, const std::vector<float> &direction)
{
    float a = initialRangeStart;
    float b = initialRangeStop;

    float x_m = (a + b) / 2.f;
    float delta = b - a;

    std::vector arg(start);

    for (size_t i=0; i < start.size(); i++)
        arg[i] += x_m*direction[i];

    float f_x_m = fun.Value(arg);

    while (true) {
        const float x_1 = a + delta/4.f;
        const float x_2 = b - delta/4.f;

        for (size_t i=0; i < start.size(); i++)
            arg[i] = start[i] + x_1*direction[i];

        const float f_x_1 = fun.Value(arg);

        for (size_t i=0; i < start.size(); i++)
            arg[i] = start[i] + x_2*direction[i];

        const float f_x_2 = fun.Value(arg);

        if (f_x_1 < f_x_m) {
            b = x_m;
            delta = b - a;
            x_m = x_1;
            f_x_m = f_x_1;
        }
        else if (f_x_2 < f_x_m) {
            a = x_m;
            delta = b - a;
            x_m = x_2;
            f_x_m = f_x_2;
        }
        else {
            a = x_1;
            b = x_2;
            delta = b - a;
        }

        if (delta <= 2*eps)
            return x_m;
    }
}
