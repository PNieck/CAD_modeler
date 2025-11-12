#pragma once

#include <algebra/vec3.hpp>


class MillingSettings {
public:
    float baseThickness = 0.17f;
    float broadPhaseAdditionalThickness = 0.02f;

    alg::Vec3 initCutterPos = alg::Vec3(0.0f, 0.66f, 0.0f);
};
