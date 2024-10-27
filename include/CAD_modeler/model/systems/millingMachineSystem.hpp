#pragma once

#include <ecs/system.hpp>

#include <algebra/mat4x4.hpp>

#include "../components/mesh.hpp"
#include "../components/millingMachinePaths.hpp"


class MillingMachineSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void CreateMaterial(int xResolution, int yResolution);

    void AddPaths(MillingMachinePaths&& paths);

    void Render(const alg::Mat4x4& view, const alg::Mat4x4& perspective);



private:
    unsigned int heightmap = 0;

    Mesh material;

    std::vector<float> GenerateVertices();
    std::vector<uint32_t> GenerateIndices();

    std::vector<float> GeneratePathsVertices(const MillingMachinePaths& paths);
    std::vector<uint32_t> GeneratePathsIndices(const MillingMachinePaths& paths);
};
