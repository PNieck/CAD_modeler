#pragma once

#include "shader.hpp"


class BicubicBezierSurfaceTrianglesShader: public Shader {
public:
    constexpr static float MaxTessellationLevel = GL_MAX_TESS_GEN_LEVEL;

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

    void SetTesselationLevel(const float level) const {
        const float v[] = { level, level, level, level };

        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);
        glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, v);
    }

};
