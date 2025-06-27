#pragma once

#include "shader.hpp"


class TrimmedBicubicBSplineSurfaceShader: public Shader {
public:
    TrimmedBicubicBSplineSurfaceShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/trimmedShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/trimmedBSplineSurfaceShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }
};