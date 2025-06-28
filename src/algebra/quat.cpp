#include <algebra/quat.hpp>

#include <cmath>
#include <algorithm>
#include <cassert>


alg::Quat::Quat(const float pitch, const float yaw, const float roll)
{
    const float cz = std::cos(roll * 0.5f);
    const float sz = std::sin(roll * 0.5f);
    const float cx = std::cos(pitch * 0.5f);
    const float sx = std::sin(pitch * 0.5f);
    const float cy = std::cos(yaw * 0.5f);
    const float sy = std::sin(yaw * 0.5f);

    W() = cx * cy * cz + sx * sy * sz;
    X() = sx * cy * cz - cx * sy * sz;
    Y() = cx * sy * cz + sx * cy * sz;
    Z() = cx * cy * sz - sx * sy * cz;
}


alg::Quat::Quat(const Vec3& axis, const float angle)
{
    Vec3 axisNorm = axis.Normalize();
    const float halfAngle = angle * 0.5f;
    const float sinHalfAngle = std::sin(halfAngle);

    X() = axisNorm.X() * sinHalfAngle;
    Y() = axisNorm.Y() * sinHalfAngle;
    Z() = axisNorm.Z() * sinHalfAngle;
    W() = std::cos(halfAngle);
}


alg::Vec3 alg::Quat::Rotate(const Vec3& v) const
{
    Quat tmp(v.X(), v.Y(), v.Z(), 0.0f);

    tmp = (*this) * tmp * this->Conjugation();

    return {
        tmp.X(),
        tmp.Y(),
        tmp.Z()
    };
}


alg::Mat4x4 alg::Quat::ToRotationMatrix() const
{
    const float xy = X() * Y();
    const float xz = X() * Z();
    const float yz = Y() * Z();
    const float wx = W() * X();
    const float wz = W() * Z();
    const float wy = W() * Y();
    
    alg::Mat4x4 rot(
        1.f - 2.f*(Y()*Y() + Z()*Z()),            2.f*(xy - wz),            2.f*(xz + wy), 0.f,
                        2.f*(xy + wz), 1.0f - 2.f*(X()*X() + Z()*Z()),            2.f*(yz - wx), 0.f,
                   2.f*(xz - wy),            2.f*(yz + wx), 1.f - 2.f*(X()*X() + Y()*Y()), 0.f,
                             0.f,                      0.f,                      0.f, 1.f
    );

    rot.TransposeSelf();

    return rot;
}


alg::Vec3 alg::Quat::ToRollPitchYaw() const
{
    float y = 2.f * (Y() * Z() + W() * X());
    float x = W() * W() - X() * X() - Y() * Y() + Z() * Z();

    float pitch;
    if (std::abs(x) <= std::numeric_limits<float>::epsilon() &&
        std::abs(y) <= std::numeric_limits<float>::epsilon())
        pitch = 2.f * std::atan2(X(), W());
    else
        pitch = std::atan2(y, x);

    const float yaw = std::asin(std::clamp(-2.f * (X() * Z() - W() * Y()), -1.f, 1.f));

    y = 2.f * (X() * Y() + W() * Z());
    x = W() * W() + X() * X() - Y() * Y() - Z() * Z();

    float roll;
    if (std::abs(x) <= std::numeric_limits<float>::epsilon() &&
        std::abs(y) <= std::numeric_limits<float>::epsilon())
        roll = 0;
    else
        roll = std::atan2(y, x);

    return {pitch, yaw, roll};
}


alg::Quat alg::Quat::operator*(const Quat& q) const
{
    return {
        W() * q.X() + X() * q.W() + Y() * q.Z() - Z() * q.Y(),  // i
        W() * q.Y() - X() * q.Z() + Y() * q.W() + Z() * q.X(),  // j
        W() * q.Z() + X() * q.Y() - Y() * q.X() + Z() * q.W(),  // k
        W() * q.W() - X() * q.X() - Y() * q.Y() - Z() * q.Z()   // 1
    };
}


alg::Quat alg::Quat::FromVectors(const Vec3 &v1, const Vec3 &v2)
{
    assert(v1.LengthSquared() != 0.f);
    assert(v2.LengthSquared() != 0.f);

    const Vec3 uv1 = v1.Normalize();
    const Vec3 uv2 = v2.Normalize();

    const float dot = Dot(uv1, uv2);

    if (dot > 0.999999f) {
        /* if vectors are close to parallel */
        return Identity();
    }
    if (dot < -0.999999f) {
        /* if vectors are close to antiparallel */
        /* calculating quaternion, which rotates 180 degrees along axis perpendicular to `v1` and `v2` */
        Vec3 perpen = GetPerpendicularVec(uv1);
        return { perpen.X(), perpen.Y(), perpen.Z(), 0.f };
    }

    auto cross = Cross(uv1, uv2);
    return Quat(
        cross.X(), cross.Y(), cross.Z(), 1.f + dot
    ).Normalize();
}
