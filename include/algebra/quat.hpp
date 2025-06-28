#pragma once

#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4x4.hpp"


namespace alg
{
    class Quat {
    public:
        Quat() = default;

        Quat(float x, float y, float z, float w):
            data(x, y, z, w) {}

        Quat(Vec4 v):
            data(v) {}

        Quat(Vec3 pitchYawRoll):
            Quat(pitchYawRoll.X(), pitchYawRoll.Y(), pitchYawRoll.Z()) {}

        Quat(float pitch, float yaw, float roll);

        Quat(const Vec3& axis, float angle);

        float& X()
            { return data.X(); }

        float& Y()
            { return data.Y(); }

        float& Z()
            { return data.Z(); }

        float& W()
            { return data.W(); }

        float X() const
            { return data.X(); }

        float Y() const
            { return data.Y(); }

        float Z() const
            { return data.Z(); }

        float W() const
            { return data.W(); }

        Vec3 Rotate(const alg::Vec3& v) const;
        
        [[nodiscard]]
        Quat Conjugation() const
            { return { -X(), -Y(), -Z(), W() }; }

        [[nodiscard]]
        Quat Normalize() const
            { return data.Normalize(); }

        [[nodiscard]]
        Mat4x4 ToRotationMatrix() const;

        [[nodiscard]]
        Vec3 ToRollPitchYaw() const;
        

        Quat operator*(const Quat& q) const;

        static Quat Identity()
            { return {0.0f, 0.0f, 0.0f, 1.0f}; }

        static Quat FromVectors(const Vec3& v1, const Vec3& v2);
        
    private:
        Vec4 data{};
    };
};
