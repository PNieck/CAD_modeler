#pragma once

#include "shader.hpp"


class BicubicBezierSurfaceTrianglesShader: public Shader {
public:
    using Shader::MaxTessellationLevel;

    BicubicBezierSurfaceTrianglesShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/bezierSurfaceTrianglesShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }

    void SetDensity(const float level) const
        { SetAllTesselationLevels(level); }
};
