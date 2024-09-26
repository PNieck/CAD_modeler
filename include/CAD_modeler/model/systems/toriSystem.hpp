#pragma once

#include <ecs/system.hpp>
#include "utils/nameGenerator.hpp"
#include "shaders/shaderRepository.hpp"
#include "../components/position.hpp"
#include "../components/torusParameters.hpp"

#include <vector>


class ToriSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shaderRepo);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetTorusParameter(Entity entity, const TorusParameters& params);

    void Render(const alg::Mat4x4& cameraMtx);

private:
    NameGenerator nameGenerator;

    ShaderRepository* shadersRepo;

    std::vector<float> GenerateMeshVertices(const TorusParameters& params) const;
    std::vector<uint32_t> GenerateMeshIndices(const TorusParameters& params) const;

    float VertexX(const TorusParameters& params, float alpha, float beta) const;
    float VertexY(const TorusParameters& params, float alpha) const;
    float VertexZ(const TorusParameters& params, float alpha, float beta) const;
};
