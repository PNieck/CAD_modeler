#include <CAD_modeler/model/components/scale.hpp>


glm::mat4x4 Scale::ScaleMatrix() const
{
    return glm::mat4x4(
        scale.x,    0.0f,    0.0f,  0.0f,
           0.0f, scale.y,    0.0f,  0.0f,
           0.0f,    0.0f, scale.z,  0.0f,
           0.0f,    0.0f,    0.0f,  1.0f
    );
}
