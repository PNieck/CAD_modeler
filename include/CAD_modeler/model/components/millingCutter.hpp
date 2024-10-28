#pragma once


class MillingCutter {
public:
    enum class Type {
        Round,
        Flat
    };

    MillingCutter(const float radius, const Type type):
        radius(radius), type(type) {}

    float radius;
    Type type;
};
