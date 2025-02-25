#pragma once

#include <ecs/system.hpp>

#include <algebra/vec3.hpp>
#include "../components/position.hpp"
#include "../components/vector.hpp"


class VectorSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity AddVector(const alg::Vec3& vec, const Position& pos);
    void AddVector(Entity entity, const alg::Vec3& vec, const Position& pos);
    void AddVector(Entity entity, const alg::Vec3& vec);

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    void UpdateMesh(Entity entity) const;

    [[nodiscard]]
    static std::vector<float> GenerateNetVertices(const Vector& vec);

    [[nodiscard]]
    static std::vector<uint32_t> GenerateNetIndices();
};
