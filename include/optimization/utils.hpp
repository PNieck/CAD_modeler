#pragma once

#include <vector>

namespace opt {
    inline float LengthSquared(const std::vector<float>& vec)
    {
        float result = 0.f;

        for (const float value: vec) {
            result += value * value;
        }

        return result;
    }
}

