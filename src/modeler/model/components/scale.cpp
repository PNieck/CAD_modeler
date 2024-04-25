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
