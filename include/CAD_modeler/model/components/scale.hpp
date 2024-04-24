#pragma once

#include <algebra/vec3.hpp>
#include <algebra/mat4x4.hpp>


class Scale {
public:
    Scale(float value = 1.0f): scale(value) {}
    Scale(float x, float y, float z): scale(x, y, z) {}
    Scale(alg::Vec3 vec): scale(vec) {}

    inline float GetX() const { return scale.X(); }
    inline float GetY() const { return scale.Y(); } 
    inline float GetZ() const { return scale.Z(); }

    inline alg::Vec3& AsVector() { return scale; }

    alg::Mat4x4 ScaleMatrix() const;

private:
    alg::Vec3 scale;
};
