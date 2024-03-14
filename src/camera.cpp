#include <CAD_modeler/camera.hpp>


const glm::vec3 Camera::global_up = glm::vec3(0.0f, 1.0f, 0.0f);


Camera::Camera(glm::vec3 position, glm::vec3 target, float aspect_ratio, float fov, float near_plane, float far_plane):
    position(position), target(target), fov(fov), aspect_ratio(aspect_ratio), near_plane(near_plane), far_plane(far_plane)
{
    glm::vec3 direction = position - target;
    right = glm::normalize(glm::cross(global_up, direction));
    up = glm::normalize(glm::cross(direction, right));
}



glm::mat4x4 Camera::GetViewMatrix() const
{
    glm::vec3 direction = glm::normalize(position - target);

    return glm::transpose(glm::mat4x4(
            right.x,     right.y,     right.z,     -position.x*right.x -     position.y*right.y -     position.z*right.z,
               up.x,        up.y,        up.z,        -position.x*up.x -        position.y*up.y -        position.z*up.z,
        direction.x, direction.y, direction.z, -position.x*direction.x - position.y*direction.y - position.z*direction.z,
               0.0f,        0.0f,        0.0f,                                                                      1.0f
    ));
}


glm::mat4x4 Camera::GetPerspectiveMatrix() const
{
    float v1 = 1.0f/std::tan(fov/2.0);
    float v2 = v1/aspect_ratio;
    float v3 = (far_plane + near_plane)/(far_plane - near_plane);
    float v4 = -2.0 * (far_plane * near_plane) / (far_plane - near_plane);

    return glm::transpose(glm::mat4x4(
        v2, 0.0f, 0.0f, 0.0f,
        0.0f, v1, 0.0f, 0.0f,
        0.0f, 0.0f, -v3, v4,
        0.0f, 0.0f, -1.0f, 0.0f
    ));
}


void Camera::TargetSet(const glm::vec3 &target)
{
    this->target = target;
}
