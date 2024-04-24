#pragma once


#include "shader.hpp"


class CubicBezierShader: private Shader {
public:
    CubicBezierShader():
        Shader(
            "../../shaders/bezierShader.vert",
            "../../shaders/stdShader.frag",
            "../../shaders/bezierShader.tesc",
            "../../shaders/bezierShader.tese"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
