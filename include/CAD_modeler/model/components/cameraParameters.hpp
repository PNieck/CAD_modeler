#pragma once

#include "position.hpp"


class CameraParameters {
public:
    Position target;
    int viewportWidth;
    int viewportHeight;
    float fov;
    float near_plane;
    float far_plane;
};
