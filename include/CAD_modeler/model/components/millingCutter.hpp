#pragma once

#include "position.hpp"


class MillingCutter {
public:
    enum class Type {
        Round,
        Flat
    };

    MillingCutter(const float radius, const Type type, const float height = 1.f):
        radius(radius), height(height), type(type) {}

    [[nodiscard]]
    float YCoordinate(const Position& pos, float x, float z) const;

    [[nodiscard]]
    float CalcMinYCoord(float cutterX, float cutterZ, const Position& pointPos) const;

    float radius;
    float height;
    Type type;
};
