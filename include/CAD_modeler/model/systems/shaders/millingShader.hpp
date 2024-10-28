#pragma once

#include "shader.hpp"


class MillingShader: private Shader {
public:
    MillingShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/phongShader.frag",
            "../../shaders/heightmapShader.tesc",
            "../../shaders/heightmapShader.tese"
        ) {}

    void SetMVP(const alg::Mat4x4& matrix) const
        { setMatrix4("MVP", matrix); }

    void SetProjection00(const float val) const
        { setFloat("projection00", val); }

    void SetMainHeightmapCorner(const alg::Vec3& coord) const
        { setVec3("mainHeightMapCorner", coord); }

    void SetHeightMapXLen(const float width) const
        { setFloat("heightMapXLen", width); }

    void SetHeightMapZLen(const float height) const
        { setFloat("heightMapZLen", height); }

    void SetCameraPosition(const alg::Vec3& pos) const
        { setVec3("cameraPos", pos); }

    void Use() const
        { use(); }
};
