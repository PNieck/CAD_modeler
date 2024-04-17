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

    inline void SetColor(const glm::vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const glm::mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
