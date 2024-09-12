#pragma once

#include "shader.hpp"


class PassThroughShader: private Shader {
public:
    PassThroughShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    inline void Use() const
        { use(); }
};
