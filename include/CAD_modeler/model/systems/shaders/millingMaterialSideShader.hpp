#pragma once

#include "shader.hpp"


class MillingMaterialSideShader: public Shader {
public:
    enum class SideType: int {
        PositiveX = 0,
        NegativeX = 1,
        PositiveZ = 2,
        NegativeZ = 3,
    };

    MillingMaterialSideShader():
        Shader(
            "../../shaders/millingMaterialSide.vert",
            "../../shaders/phongShader.frag"
        ) {}

    void SetVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("VP", matrix); }

    void SetNormal(const alg::Vec3& normal) const
        { SetVec3("inNormal", normal); }

    void SetCameraPosition(const alg::Vec3& pos) const
        { SetVec3("cameraPos", pos); }

    void SetMainHeightmapCorner(const alg::Vec3& coord) const
        { SetVec3("mainHeightMapCorner", coord); }

    void SetHeightMapXLen(const float width) const
        { SetFloat("heightMapXLen", width); }

    void SetHeightMapZLen(const float height) const
        { SetFloat("heightMapZLen", height); }

    void SetSideType(SideType type) const
        { SetInt("sideType", static_cast<int>(type)); }
};
