#pragma once

#include "shader.hpp"


class TrimmedTorusShader : public Shader {
public:
    TrimmedTorusShader():
        Shader(
            "../../shaders/trimmedTorusShader.vert",
            "../../shaders/trimmedTorusShader.frag"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }
};
