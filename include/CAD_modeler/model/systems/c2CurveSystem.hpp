#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c2CurveParameters.hpp"
#include "../components/curveControlPoints.hpp"
#include "utils/nameGenerator.hpp"
#include "curveControlPointsSystem.hpp"


class C2CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateC2Curve(const std::vector<Entity>& entities);
    inline Entity CreateC2Curve(Entity entity)
        { CreateC2Curve({entity}); }

    inline void AddControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity); }

    inline void DeleteControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity); }

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    void UpdateEntities() const;
    void UpdateMesh(Entity curve) const;

    std::vector<float> GenerateCurveMeshVertices(const CurveControlPoints& params) const;
    std::vector<uint32_t> GenerateCurveMeshIndices(const CurveControlPoints& params) const;
};
