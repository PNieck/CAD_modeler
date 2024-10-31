#pragma once


class MillingCutter {
public:
    enum class Type {
        Round,
        Flat
    };

    MillingCutter(const float radius, const Type type, float height = 1.f):
        radius(radius), type(type), height(height) {}

    float radius;
    float height;
    Type type;
};
