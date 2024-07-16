#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/controlPoints.hpp"
#include "../components/c0CurveParameters.hpp"
#include "../components/position.hpp"
#include "utils/nameGenerator.hpp"
#include "controlPointsSystem.hpp"

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
        { coordinator->GetSystem<ControlPointsSystem>()->AddControlPoint(bezierCurve, entity); }

    inline void DeleteControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<ControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity); }

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static constexpr int CONTROL_POINTS_PER_SEGMENT = 4;

    void RenderCurvesPolygons(std::stack<Entity>& entities) const;
    void UpdateEntities() const;
    void UpdateMesh(Entity curve, const ControlPoints& cps) const;

    std::vector<float> GenerateBezierPolygonVertices(const ControlPoints& cps) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const ControlPoints& cps) const;
};
