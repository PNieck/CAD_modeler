#pragma once


#include "shader.hpp"


class CubicBezierCurveShader: public Shader {
public:
    CubicBezierCurveShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            "../../shaders/bezierCurveShader.tesc",
            "../../shaders/bezierCurveShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }
};
