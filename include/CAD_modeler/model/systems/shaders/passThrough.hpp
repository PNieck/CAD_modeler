#pragma once

#include "shader.hpp"


class PassThroughShader: public Shader {
public:
    PassThroughShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { SetVec4("color", color); }
};
