#pragma once

#include <algorithm>


class LinearInterpolator {
public:
    LinearInterpolator(const float v1, const float v2, const float t=1.f):
        v1(v1), v2(v2), t(t) {}

    float Interpolate(float actT) const
    {
        actT /= this->t;
        actT = std::clamp(actT, 0.f, 1.f);

        return (1.f - actT) * v1 + actT * v2;
    }


private:
    float v1, v2;
    float t;
};
