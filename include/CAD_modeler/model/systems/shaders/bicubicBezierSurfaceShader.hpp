#pragma once

#include "shader.hpp"


class BicubicBezierSurfaceShader: public Shader {
public:
    BicubicBezierSurfaceShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/bezierSurfaceIsolinesShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }
};
