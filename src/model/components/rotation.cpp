#include <CAD_modeler/model/components/rotation.hpp>


glm::mat4x4 Rotation::GetRotationMatrix() const
{
    return RotationXMtx() * RotationYMtx() * RotationZMtx();
}


glm::mat4x4 Rotation::RotationXMtx() const
{
    float cos_a = std::cosf(rot.x);
    float sin_a = std::sinf(rot.x);

    return glm::transpose(glm::mat4x4(
        1.0f,  0.0f,   0.0f,  0.0f,
        0.0f, cos_a, -sin_a,  0.0f,
        0.0f, sin_a,  cos_a,  0.0f,
        0.0f,  0.0f,   0.0f,  1.0f
    ));
}


glm::mat4x4 Rotation::RotationYMtx() const
{
    float cos_a = std::cosf(rot.y);
    float sin_a = std::sinf(rot.y);

    return glm::transpose(glm::mat4x4(
        cos_a,  0.0f, sin_a,  0.0f,
         0.0f,  1.0f,  0.0f,  0.0f,
        -sin_a, 0.0f,  cos_a, 0.0f,
         0.0f,  0.0f,  0.0f,  1.0f
    ));
}


glm::mat4x4 Rotation::RotationZMtx() const
{
    float cos_a = std::cosf(rot.z);
    float sin_a = std::sinf(rot.z);

    return glm::transpose(glm::mat4x4(
        cos_a,  -sin_a, 0.0f,  0.0f,
         sin_a,  cos_a,  0.0f,  0.0f,
         0.0f, 0.0f,  1.0f, 0.0f,
         0.0f,  0.0f,  0.0f,  1.0f
    ));
}
