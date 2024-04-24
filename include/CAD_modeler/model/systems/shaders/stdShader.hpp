#pragma once

#include "shader.hpp"


class StdShader: private Shader {
public:
    StdShader():
        Shader(
            "../../shaders/stdShader.vert",
            "../../shaders/stdShader.frag"
        ) {}

    inline void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
