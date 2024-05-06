#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/curveControlPoints.hpp"
#include "../components/c0CurveParameters.hpp"
#include "../components/position.hpp"
#include "utils/nameGenerator.hpp"
#include "curveControlPointsSystem.hpp"

#include <vector>
#include <tuple>
#include <stack>


class C0CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateC0Curve(const std::vector<Entity>& entities);
    inline Entity CreateC0Curve(Entity entity)
        { CreateC0Curve({entity}); }

    inline void AddControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity); }

    inline void DeleteControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity); }

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static constexpr int CONTROL_POINTS_PER_SEGMENT = 4;

    void RenderCurvesPolygons(std::stack<Entity>& entities) const;
    void UpdateEntities() const;
    void UpdateMesh(Entity curve) const;

    std::vector<float> GenerateBezierPolygonVertices(const CurveControlPoints& params) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const CurveControlPoints& params) const;
};
