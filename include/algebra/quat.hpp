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

        Quat(const alg::Vec3& axis, float angle);

        inline float& X()
            { return data.X(); }

        inline float& Y()
            { return data.Y(); }

        inline float& Z()
            { return data.Z(); }

        inline float& W()    
            { return data.W(); }

        inline float X() const
            { return data.X(); }

        inline float Y() const
            { return data.Y(); }

        inline float Z() const
            { return data.Z(); }

        inline float W() const  
            { return data.W(); }

        alg::Vec3 Rotate(const alg::Vec3& v) const;
        
        Quat Conjugation() const
            { return { -X(), -Y(), -Z(), W() }; }

        inline Quat Normalize() const
            { return Quat(data.Normalize()); }

        alg::Mat4x4 ToRotationMatrix() const;
        alg::Vec3 ToRollPitchYaw() const;
        

        Quat operator*(const Quat& q) const;

        static Quat Identity()
            { return Quat(0.0f, 0.0f, 0.0f, 1.0f); }

        static Quat FromVectors(const Vec3& v1, const Vec3& v2);
        
    private:
        Vec4 data;
    };
};
