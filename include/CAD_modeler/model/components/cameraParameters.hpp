#pragma once

#include "position.hpp"


class CameraParameters {
public:
    Position target;
    int viewportWidth;
    int viewportHeight;
    float fov;          // Vertical fov
    float nearPlane;
    float farPlane;

    [[nodiscard]]
    float GetAspectRatio() const
        { return static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight); }
};
