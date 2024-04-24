#include <algebra/quat.hpp>

#include <cmath>
#include <numbers>


alg::Quat::Quat(float pitch, float yaw, float roll)
{
    float cr = cos(roll * 0.5);
    float sr = sin(roll * 0.5);
    float cp = cos(pitch * 0.5);
    float sp = sin(pitch * 0.5);
    float cy = cos(yaw * 0.5);
    float sy = sin(yaw * 0.5);

    W() = cr * cp * cy + sr * sp * sy;
    X() = sr * cp * cy - cr * sp * sy;
    Y() = cr * sp * cy + sr * cp * sy;
    Z() = cr * cp * sy - sr * sp * cy;
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
        2.f*(w2 + X()*X()) - 1.f,            2.f*(xy - wz),            2.f*(xz + wz), 0.f,
                   2.f*(xy + wz), 2.f*(w2 + Y()*Y()) - 1.f,            2.f*(yz - wx), 0.f,
                   2.f*(xz - wy),            2.f*(yz + wx), 2.f*(w2 + Y()*Y()) - 1.f, 0.f,
                             0.f,                      0.f,                      0.f, 1.f
    );

    rot.TransposeSelf();

    return rot;
}


alg::Vec3 alg::Quat::ToRollPitchRoll() const
{
    // roll (x-axis rotation)
    float sinr_cosp = 2 * (W() * X() + Y() * Z());
    float cosr_cosp = 1 - 2 * (X() * X() + Y() * Y());
    float roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = std::sqrt(1 + 2 * (W() * Y() - X() * Z()));
    float cosp = std::sqrt(1 - 2 * (W() * Y() - X() * Z()));
    float pitch = 2 * std::atan2(sinp, cosp) - std::numbers::pi_v<float> / 2.0f;

    // yaw (z-axis rotation)
    float siny_cosp = 2 * (W() * Z() + X() * Y());
    float cosy_cosp = 1 - 2 * (Y() * Y() + Z() * Z());
    float yaw = std::atan2(siny_cosp, cosy_cosp);

    return alg::Vec3(roll, pitch, roll);
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
