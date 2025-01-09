#pragma once

#include <numbers>


class Angle {
public:
    static Angle FromDegrees(const float degrees)
        { return Angle(degrees); }

    static Angle FromRadians(const float radians)
        { return Angle(radians*radiansToDegreesConst); }

    [[nodiscard]]
    float ToDegrees() const
        { return degrees; }

    [[nodiscard]]
    float ToRadians() const
        { return degrees * degreeToRadiansConst; }

private:
    explicit Angle(const float degrees): degrees(degrees) {}

    static constexpr float degreeToRadiansConst = std::numbers::pi_v<float> / 180.f;
    static constexpr float radiansToDegreesConst = 1.f / degreeToRadiansConst;

    float degrees;
};
