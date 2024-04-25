#include <algebra/quat.hpp>

#include <cmath>
#include <numbers>
#include <algorithm>


alg::Quat::Quat(float pitch, float yaw, float roll)
{
    float cz = cos(roll * 0.5);
    float sz = sin(roll * 0.5);
    float cx = cos(pitch * 0.5);
    float sx = sin(pitch * 0.5);
    float cy = cos(yaw * 0.5);
    float sy = sin(yaw * 0.5);

    W() = cx * cy * cz + sx * sy * sz;
    X() = sx * cy * cz - cx * sy * sz;
    Y() = cx * sy * cz + sx * cy * sz;
    Z() = cx * cy * sz - sx * sy * cz;
}


alg::Vec3 alg::Quat::Rotate(const alg::Vec3& v) const
{
    alg::Quat tmp(v.X(), v.Y(), v.Z(), 0.0f);

    tmp = (*this) * tmp * this->Conjugation();

    return alg::Vec3(
        tmp.X(),
        tmp.Y(),
        tmp.Z()
    );
}


alg::Quat alg::Quat::Conjugation() const
{
    return Quat(
        -X(), -Y(), -Z(), W()
    );
}


alg::Mat4x4 alg::Quat::ToRotationMatrix() const
{
    float w2 = W() * W();
    float xy = X() * Y();
    float xz = X() * Z();
    float yz = Y() * Z();
    float wx = W() * X();
    float wz = W() * Z();
    float wy = W() * Y();
    
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

    float yaw = std::asin(std::clamp(-2.f * (X() * Z() - W() * Y()), -1.f, 1.f));

    y = 2.f * (X() * Y() + W() * Z());
    x = W() * W() + X() * X() - Y() * Y() - Z() * Z();

    float roll;
    if (std::abs(x) <= std::numeric_limits<float>::epsilon() &&
        std::abs(y) <= std::numeric_limits<float>::epsilon())
        roll = 0;
    else
        roll = std::atan2(y, x);

    return alg::Vec3(pitch, yaw, roll);
}


alg::Quat alg::Quat::operator*(const alg::Quat& q) const
{
    return Quat(
        W() * q.X() + X() * q.W() + Y() * q.Z() - Z() * q.Y(),  // i
        W() * q.Y() - X() * q.Z() + Y() * q.W() + Z() * q.X(),  // j
        W() * q.Z() + X() * q.Y() - Y() * q.X() + Z() * q.W(),  // k
        W() * q.W() - X() * q.X() - Y() * q.Y() - Z() * q.Z()   // 1
    );
}
