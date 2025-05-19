#pragma once

#include "shader.hpp"


class MillingShader: public Shader {
public:
    MillingShader():
        Shader(
            "../../shaders/passThroughShader.vert",
            "../../shaders/phongShader.frag",
            "../../shaders/heightmapShader.tesc",
            "../../shaders/heightmapShader.tese"
        ) {}

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }

    void SetProjection00(const float val) const
        { SetFloat("projection00", val); }

    void SetMainHeightmapCorner(const alg::Vec3& coord) const
        { SetVec3("mainHeightMapCorner", coord); }

    void SetHeightMapXLen(const float width) const
        { SetFloat("heightMapXLen", width); }

    void SetHeightMapZLen(const float height) const
        { SetFloat("heightMapZLen", height); }

    void SetCameraPosition(const alg::Vec3& pos) const
        { SetVec3("cameraPos", pos); }
};
