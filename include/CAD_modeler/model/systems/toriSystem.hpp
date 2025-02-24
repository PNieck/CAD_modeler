#pragma once

#include <ecs/system.hpp>
#include "shaders/shaderRepository.hpp"
#include "../components/position.hpp"
#include "../components/rotation.hpp"
#include "../components/scale.hpp"
#include "../components/torusParameters.hpp"

#include <vector>


class ToriSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    void Init(ShaderRepository* shaderRepo);

    Entity AddTorus(const Position& pos, const TorusParameters& params);

    void SetParameters(Entity entity, const TorusParameters& params);

    void Render(const alg::Mat4x4& cameraMtx);

    Position PointOnTorus(Entity torus, float alpha, float beta) const;

    static alg::Vec3 PointOnTorus(const TorusParameters& params, float alpha, float beta);

    static Position PointOnTorus(
        const TorusParameters& params,
        const Position& pos,
        const Rotation& rot,
        const Scale& scale,
        float alpha,
        float beta
    );

    static alg::Vec3 PartialDerivativeWithRespectToAlpha(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float alpha,
        float beta
    );
    [[nodiscard]] alg::Vec3 PartialDerivativeWithRespectToAlpha(Entity e, float alpha, float beta) const;

    static alg::Vec3 PartialDerivativeWithRespectToBeta(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float alpha,
        float beta
    );
    alg::Vec3 PartialDerivativeWithRespectToBeta(Entity e, float alpha, float beta) const;

    static alg::Vec3 NormalVec(
        const TorusParameters &params,
        const Rotation &rot,
        const Scale &scale,
        float alpha,
        float beta
    );
    alg::Vec3 NormalVec(Entity e, float alpha, float beta) const;

private:
    ShaderRepository* shadersRepo;

    void UpdateMesh(Entity e, const TorusParameters& params) const;

    static std::vector<float> GenerateMeshVertices(const TorusParameters& params);
    static std::vector<uint32_t> GenerateMeshIndices(const TorusParameters& params);
};
