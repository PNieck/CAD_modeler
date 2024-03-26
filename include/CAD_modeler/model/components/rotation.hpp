#ifndef ROTATION_H
#define ROTATION_H

#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>


class Rotation {
public:
    Rotation(): rot(0) {}

    glm::vec3 rot;

    glm::mat4x4 GetRotationMatrix() const;

private:
    glm::mat4x4 RotationXMtx() const;
    glm::mat4x4 RotationYMtx() const;
    glm::mat4x4 RotationZMtx() const;
};


#endif
