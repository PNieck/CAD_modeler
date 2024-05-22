#pragma once

#include "shader.hpp"


class BicubicBezierSurfaceShader: private Shader {
public:
    BicubicBezierSurfaceShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            nullptr, // No tesselation control shader
            "../../shaders/bezierSurfaceShader.tese"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
