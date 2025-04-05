#pragma once


class MillingCutter {
public:
    enum class Type {
        Round,
        Flat
    };

    MillingCutter(const float radius, const Type type, const float height = 1.f):
        radius(radius), height(height), type(type) {}

    float radius;
    float height;
    Type type;
};
