#ifndef ROTATION_H
#define ROTATION_H

#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_float.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


class Rotation {
public:
    Rotation(): quat(1.0f, 0.0f, 0.0f, 0.0f) {}
    Rotation(glm::vec3 pitchYawRoll): quat(pitchYawRoll) {}
    Rotation(float pitch, float yaw, float roll):
        Rotation(glm::vec3(pitch, yaw, roll)) {}
    Rotation(const glm::quat& rot): quat(rot) {}

    inline void Rotate(glm::vec3& vec) const
        { vec = quat * vec; }

    inline glm::mat4x4 GetRotationMatrix() const
        { return glm::toMat4(quat); }

    inline glm::vec3 GetEulerAngles() const
        { return glm::eulerAngles(quat); }

    glm::quat quat;
private:
    // glm::mat4x4 RotationXMtx() const;
    // glm::mat4x4 RotationYMtx() const;
    // glm::mat4x4 RotationZMtx() const;

    
};


#endif
