#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>


class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 target, float aspect_ratio, float fov = glm::radians(45.0f), float near_plane = 0.1f, float far_plane = 100.0f);

    glm::mat4x4 GetViewMatrix()const;
    glm::mat4x4 GetPerspectiveMatrix() const;

private:
    static const glm::vec3 global_up;

    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 direction;

    float fov;
    float aspect_ratio;
    float near_plane;
    float far_plane;
};

#endif
