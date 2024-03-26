#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "system.hpp"

#include <glm/mat4x4.hpp>


class CameraSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(int viewport_width, int viewport_height);

    glm::mat4x4 ViewMatrix() const;
    glm::mat4x4 PerspectiveMatrix() const;

private:
    static constexpr glm::vec3 globalUp = glm::vec3(0.0f, 1.0f, 0.0f);

    Entity camera;
};


#endif
