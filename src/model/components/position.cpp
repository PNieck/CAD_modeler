#include <CAD_modeler/model/components/position.hpp>


glm::mat4x4 Position::TranslationMatrix() const
{
    return glm::mat4x4(
          1.0f,   0.0f,   0.0f,  0.0f,
          0.0f,   1.0f,   0.0f,  0.0f,
          0.0f,   0.0f,   1.0f,  0.0f,
         vec.x,  vec.y,  vec.z,  1.0f
    );
}
