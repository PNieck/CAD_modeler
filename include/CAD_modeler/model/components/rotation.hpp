#ifndef ROTATION_H
#define ROTATION_H

#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


class Rotation {
public:
    Rotation(): quat() {}
    Rotation(float pitch, float yaw, float roll):
        quat(glm::vec3(pitch, yaw, roll)) {}

    inline void Rotate(glm::vec3& vec) const
        { vec = quat * vec; }

    inline glm::mat4x4 GetRotationMatrix() const
        { return glm::toMat4(quat); }

private:
    // glm::mat4x4 RotationXMtx() const;
    // glm::mat4x4 RotationYMtx() const;
    // glm::mat4x4 RotationZMtx() const;

    glm::quat quat;
};


#endif
