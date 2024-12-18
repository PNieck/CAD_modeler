#pragma once

#include "shader.hpp"


class GridShader: Shader {
public:
    GridShader():
        Shader(
            "../../shaders/gridShader.vert",
            "../../shaders/gridShader.frag"
        ) {}

    void SetFarPlane(float farPlane) const
        { SetFloat("far", farPlane); }

    void SetNearPlane(float nearPlane) const
        { SetFloat("near", nearPlane); }

    void SetViewMatrix(const alg::Mat4x4& viewMtx) const
        { SetMatrix4("view", viewMtx); }

    void SetProjectionMatrix(const alg::Mat4x4& projMtx) const
        { SetMatrix4("projection", projMtx); }

    void Use() const
        { Shader::Use(); }
};
