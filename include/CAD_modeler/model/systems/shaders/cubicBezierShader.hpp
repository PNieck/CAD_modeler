#pragma once


#include "shader.hpp"


class CubicBezierCurveShader: private Shader {
public:
    CubicBezierCurveShader():
        Shader(
            "../../shaders/bezierCurveShader.vert",
            "../../shaders/stdShader.frag",
            "../../shaders/bezierCurveShader.tesc",
            "../../shaders/bezierCurveShader.tese"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
