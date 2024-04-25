#pragma once

#include <numbers>


class Angle {
public:
    inline static Angle FromDegrees(float degrees)
        { return Angle(degrees); }

    inline static Angle FromRadians(float radians)
        { return Angle(radians*radiansToDegreesConst); }

    inline float ToDegrees() const
        { return degrees; }

    inline float ToRadians() const
        { return degrees * degreeToRadiansConst; }

private:
    Angle(float degrees): degrees(degrees) {}

    static constexpr float degreeToRadiansConst = std::numbers::pi_v<float> / 180.f;
    static constexpr float radiansToDegreesConst = 1.f / degreeToRadiansConst;

    float degrees;
};
