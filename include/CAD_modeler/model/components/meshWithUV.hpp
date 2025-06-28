#pragma once

#include "mesh.hpp"


class MeshWithUV: Mesh {
public:
    void Update(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
    {
        Mesh::Update(vertices, indices);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)(sizeof(float)*3));
    }

    [[nodiscard]]
    int GetElementsCnt() const
        { return Mesh::GetElementsCnt(); }

    void Use() const
        { Mesh::Use(); }
};
