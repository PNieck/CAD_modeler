#pragma once

#include <algebra/mat4x4.hpp>


class Position {
public:
    Position(): vec() {}
    explicit Position(const float val): vec(val) {}
    Position(const alg::Vec3& vec): vec(vec) {}
    Position(const float x, const float y, const float z): vec(x, y, z) {}

    alg::Mat4x4 TranslationMatrix() const
        { return TranslationMtx(vec); }

    float GetX() const { return vec.X(); }
    float GetY() const { return vec.Y(); }
    float GetZ() const { return vec.Z(); }

    void SetX(const float x) { vec.X() = x; }
    void SetY(const float y) { vec.Y() = y; }
    void SetZ(const float z) { vec.Z() = z; }

    alg::Vec3 vec;
};
