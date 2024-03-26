#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include <glm/vec3.hpp>


class CameraParameters {
public:
    glm::vec3 target;
    float aspect_ratio;
    float fov;
    float near_plane;
    float far_plane;
};


#endif
