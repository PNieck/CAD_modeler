#include <CAD_modeler/model/components/position.hpp>


alg::Mat4x4 Position::TranslationMatrix() const
{
    return alg::Mat4x4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        vec.X(), vec.Y(), vec.Z(), 1.0f
    );
}
