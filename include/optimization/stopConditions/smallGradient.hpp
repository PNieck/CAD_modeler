#pragma once

#include "../stopCondition.hpp"


namespace opt {
    class SmallGradient final : public StopCondition {
    public:
        explicit SmallGradient(const float eps = 1e-7f): eps(eps) {}

        bool ShouldStop(FunctionToOptimize& fun, const std::vector<float>& args) override;

    private:
        float eps;
    };
}