#include <CAD_modeler/objects/object.hpp>

#include <cmath>


Object::Object():
    position(0.0f), rotation(0.0f), scale(1.0f)
{}


glm::mat4x4 Object::model_matrix() const
{
    return translation_matrix() * rotation_matrix() * scale_matrix();
}


glm::mat4x4 Object::scale_matrix() const
{
    return glm::mat4x4(
        scale,  0.0f,  0.0f,  0.0f,
         0.0f, scale,  0.0f,  0.0f,
         0.0f,  0.0f, scale,  0.0f,
         0.0f,  0.0f,  0.0f,  1.0f
    );
}


glm::mat4x4 Object::rotation_matrix() const
{
    return rotation_x_matrix() * rotation_y_matrix() * rotation_z_matrix();
}


glm::mat4x4 Object::translation_matrix() const
{
    return glm::transpose(glm::mat4x4(
         1.0f,  0.0f,  0.0f,  position.x,
         0.0f,  1.0f,  0.0f,  position.y,
         0.0f,  0.0f,  1.0f,  position.z,
         0.0f,  0.0f,  0.0f,  1.0f
    ));
}


glm::mat4x4 Object::rotation_x_matrix() const
{
    float cos_a = std::cosf(rotation.x);
    float sin_a = std::sinf(rotation.x);

    return glm::transpose(glm::mat4x4(
        1.0f,  0.0f,   0.0f,  0.0f,
        0.0f, cos_a, -sin_a,  0.0f,
        0.0f, sin_a,  cos_a,  0.0f,
        0.0f,  0.0f,   0.0f,  1.0f
    ));
}


glm::mat4x4 Object::rotation_y_matrix() const
{
    float cos_a = std::cosf(rotation.y);
    float sin_a = std::sinf(rotation.y);

    return glm::transpose(glm::mat4x4(
        cos_a,  0.0f, sin_a,  0.0f,
         0.0f,  1.0f,  0.0f,  0.0f,
        -sin_a, 0.0f,  cos_a, 0.0f,
         0.0f,  0.0f,  0.0f,  1.0f
    ));
}


glm::mat4x4 Object::rotation_z_matrix() const
{
    float cos_a = std::cosf(rotation.z);
    float sin_a = std::sinf(rotation.z);

    return glm::transpose(glm::mat4x4(
        cos_a,  -sin_a, 0.0f,  0.0f,
         sin_a,  cos_a,  0.0f,  0.0f,
         0.0f, 0.0f,  1.0f, 0.0f,
         0.0f,  0.0f,  0.0f,  1.0f
    ));
}
