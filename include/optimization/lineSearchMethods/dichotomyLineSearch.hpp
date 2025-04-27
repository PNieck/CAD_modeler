#pragma once

#include "../lineSearch.hpp"


namespace opt
{
    class DichotomyLineSearch: public LineSearchMethod {
    public:
        DichotomyLineSearch(float rangeStart, float rangeStop, float eps):
            initialRangeStart(rangeStart), initialRangeStop(rangeStop), eps(eps) {}
    
        float Search(FunctionToOptimize& fun, const std::vector<float>& start, const std::vector<float>& direction) override;
    
    private:
        float initialRangeStart, initialRangeStop;
        float eps;
    };
}
