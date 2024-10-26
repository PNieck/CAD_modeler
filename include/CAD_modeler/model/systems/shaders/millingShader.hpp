#pragma once

#include "shader.hpp"


class MillingShader: private Shader {
public:
    MillingShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/stdShader.frag",
            "../../shaders/heightmapShader.tesc",
            "../../shaders/heightmapShader.tese"
        ) {}

    void SetColor(const alg::Vec4& color) const
        { setVec4("color", color); }

    void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    // void SetViewMtx(const alg::Mat4x4& matrix) const
    //     { setMatrix4("view", matrix); }

    void SetProjection00(const float val) const
        { setFloat("projection00", val); }

    void SetMainHeightmapCorner(const alg::Vec3& coord) const
        { setVec3("mainHeightMapCorner", coord); }

    void SetHeightMapWidth(const float width) const
        { setFloat("heightMapWidth", width); }

    void SetHeightMapLength(const float height) const
        { setFloat("heightMapLength", height); }

    void Use() const
        { use(); }
};
