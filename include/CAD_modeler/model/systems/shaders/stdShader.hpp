#pragma once

#include "shader.hpp"


class StdShader: private Shader {
public:
    StdShader():
        Shader(
            "../../shaders/stdShader.vert",
            "../../shaders/stdShader.frag"
        ) {}

    inline void SetColor(const glm::vec4& color) const
        { setVec4("color", color); }

    inline void SetMVP(const glm::mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    inline void Use() const
        { use(); }
};
