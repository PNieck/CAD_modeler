#pragma once

#include "shader.hpp"


class BicubicBSplineSurfaceShader: private Shader {
public:
    BicubicBSplineSurfaceShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/bSplineSurfaceShader.tese"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }

    inline void Use() const
        { Shader::Use(); }
};
