#pragma once

#include "../shader.hpp"


class MillingMaterialBottomShader: public Shader {
public:
    MillingMaterialBottomShader():
        Shader(
            "../../shaders/constNormalShader.vert",
            "../../shaders/phongShader.frag"
        ) {}

    void SetVP(const alg::Mat4x4& matrix) const
        { SetMatrix4("VP", matrix); }

    void SetNormal(const alg::Vec3& normal) const
        { SetVec3("globNormal", normal); }

    void SetCameraPosition(const alg::Vec3& pos) const
        { SetVec3("cameraPos", pos); }
};
