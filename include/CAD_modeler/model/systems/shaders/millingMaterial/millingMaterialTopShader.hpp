#pragma once

#include "../shader.hpp"


class MillingMaterialTopShader: public Shader {
public:
    MillingMaterialTopShader():
        Shader(
            "../../shaders/heightMapShader.vert",
            "../../shaders/phongShader.frag"
        ) {}

    void SetMVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("MVP", matrix); }

    void SetMainHeightmapCorner(const alg::Vec3& coord) const
        { SetVec3("mainHeightMapCorner", coord); }

    void SetHeightMapXLen(const float width) const
        { SetFloat("heightMapXLen", width); }

    void SetHeightMapZLen(const float height) const
        { SetFloat("heightMapZLen", height); }

    void SetCameraPosition(const alg::Vec3& pos) const
        { SetVec3("cameraPos", pos); }
};
