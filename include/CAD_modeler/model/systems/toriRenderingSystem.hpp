#pragma once

#include <ecs/system.hpp>

#include "../components/torusParameters.hpp"

#include <algebra/mat4x4.hpp>


class ToriRenderingSystem final : public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void AddEntity(Entity e);

    void RemoveEntity(Entity e);

    void Render(const alg::Mat4x4& cameraMtx);

    void Update(Entity e);
private:
    static std::vector<float> GenerateMeshVertices(const TorusParameters& params);
    static std::vector<uint32_t> GenerateMeshIndices(const TorusParameters& params);
};
