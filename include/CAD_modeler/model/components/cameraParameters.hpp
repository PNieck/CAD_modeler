#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include "position.hpp"


class CameraParameters {
public:
    Position target;
    float aspect_ratio;
    float fov;
    float near_plane;
    float far_plane;
};


#endif
