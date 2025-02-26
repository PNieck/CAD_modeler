#pragma once

#include <algebra/quat.hpp>
#include <algebra/vec3.hpp>
#include <algebra/mat4x4.hpp>


class Rotation {
public:
    Rotation(): quat(alg::Quat::Identity()) {}
    explicit Rotation(const alg::Vec3& pitchYawRoll): quat(pitchYawRoll) {}
    Rotation(float pitch, float yaw, float roll):
        quat(pitch, yaw, roll) {}
    Rotation(const alg::Quat& rot): quat(rot) {}
    Rotation(const alg::Vec3& axis, float angle): quat(axis, angle) {}

    inline void Rotate(alg::Vec3& vec) const
        { vec = quat.Rotate(vec); }

    [[nodiscard]]
    alg::Mat4x4 GetRotationMatrix() const
        { return quat.ToRotationMatrix(); }

    [[nodiscard]]
    alg::Vec3 GetRollPitchRoll() const
        { return quat.ToRollPitchYaw(); }

    [[nodiscard]]
    alg::Quat GetQuaternion() const
        { return quat; }

private:
    alg::Quat quat;
};
