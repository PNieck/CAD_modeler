#pragma once

#include <algebra/vec2.hpp>

#include <vector>


class CurveWithAABB {
public:
    CurveWithAABB(const std::vector<alg::Vec2>& points, float borderV, float offsetV);

    [[nodiscard]]
    bool Inside(float u, float v) const;

private:
    std::vector<alg::Vec2> points;
    float maxU, maxV;
    float minU, minV;

    float borderV, offsetV;
};
