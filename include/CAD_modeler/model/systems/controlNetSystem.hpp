#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include <algebra/mat4x4.hpp>

#include "../components/patches.hpp"
#include "../components/controlNetMesh.hpp"


class ControlNetSystem final : public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void AddControlPointsNet(Entity entity, const Patches& patches);

    bool HasControlPointsNet(const Entity entity) const
        { return entities.contains(entity); }

    void DeleteControlPointsNet(const Entity entity) const
        { coordinator->DeleteComponent<ControlNetMesh>(entity); }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update(Entity entity, const Patches& patches);

private:
    std::vector<float> GenerateVertices(const Patches& patches) const;
    std::vector<uint32_t> GenerateIndices(const Patches& patches) const;
};
