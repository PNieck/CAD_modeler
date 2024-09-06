#pragma once

#include <algebra/vec3.hpp>


class Vector: public alg::Vec3 {
public:
    Vector(const alg::Vec3& vec):
        alg::Vec3(vec) {}
};
