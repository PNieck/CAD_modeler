#pragma once

#include "shader.hpp"


class TrimmedBicubicBezierSurfaceShader: public Shader {
public:
    TrimmedBicubicBezierSurfaceShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/trimmedShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/trimmedBezierSurfaceShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }

    void SetSurfaceSize(const int rows, const int cols) const {
        SetInt("surfaceRows", rows);
        SetInt("surfaceCols", cols);
    }
};
