#pragma once

#include "mesh.hpp"


class MeshWithNormals: private Mesh {
public:
    MeshWithNormals() {}

    void Update(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
    {
        Mesh::Update(vertices, indices);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)(sizeof(float)*3));
    }

    [[nodiscard]]
    int GetElementsCnt() const
        { return Mesh::GetElementsCnt(); }

    void Use() const
        { Mesh::Use(); }
};
