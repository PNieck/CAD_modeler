#pragma once

#include "shader.hpp"


class GridShader: private Shader {
public:
    GridShader():
        Shader(
            "../../shaders/gridShader.vert",
            "../../shaders/gridShader.frag"
        ) {}

    inline void SetFarPlane(float farPlane) const
        { setFloat("far", farPlane); }

    inline void SetNearPlane(float nearPlane) const
        { setFloat("near", nearPlane); }

    inline void SetViewMatrix(const glm::mat4& viewMtx) const
        { setMatrix4("view", viewMtx); }

    inline void SetProjectionMatrix(const glm::mat4& projMtx) const
        { setMatrix4("proj", projMtx); }

    inline void Use() const
        { use(); }
};
