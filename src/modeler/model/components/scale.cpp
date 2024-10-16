#include <CAD_modeler/model/components/scale.hpp>


alg::Mat4x4 Scale::ScaleMatrix() const
{
    return alg::Mat4x4(
        scale.X(),      0.0f,      0.0f,  0.0f,
             0.0f, scale.Y(),      0.0f,  0.0f,
             0.0f,      0.0f, scale.Z(),  0.0f,
             0.0f,      0.0f,      0.0f,  1.0f
    );
}


void Scale::TransformVector(alg::Vec3 &vec) const
{
    vec.X() *= scale.X();
    vec.Y() *= scale.Y();
    vec.Z() *= scale.Z();
}

void Scale::TransformNormal(alg::Vec3 &vec) const
{
    vec.X() /= scale.X();
    vec.Y() /= scale.Y();
    vec.Z() /= scale.Z();
}
