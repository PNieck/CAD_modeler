#pragma once

#include <algebra/mat4x4.hpp>


class Position {
public:
    Position(): vec() {}
    Position(float val): vec(val) {}
    Position(const alg::Vec3& vec): vec(vec) {}
    Position(float x, float y, float z): vec(x, y, z) {}

    inline alg::Mat4x4 TranslationMatrix() const
        { return alg::TranslationMtx(vec); }

    inline float GetX() const { return vec.X(); }
    inline float GetY() const { return vec.Y(); } 
    inline float GetZ() const { return vec.Z(); }

    inline void SetX(float x) { vec.X() = x; }
    inline void SetY(float y) { vec.Y() = y; }
    inline void SetZ(float z) { vec.Z() = z; }

    alg::Vec3 vec;
};
