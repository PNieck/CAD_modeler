#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c0CurveParameters.hpp"
#include "../components/position.hpp"
#include "utils/nameGenerator.hpp"

#include <vector>
#include <tuple>
#include <stack>


class C0CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateBezierCurve(const std::vector<Entity>& entities);
    inline Entity CreateBezierCurve(Entity entity)
        { CreateBezierCurve({entity}); }

    void AddControlPoint(Entity bezierCurve, Entity entity);
    void DeleteControlPoint(Entity bezierCurve, Entity entity);

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static constexpr int CONTROL_POINTS_PER_SEGMENT = 4;

    void RenderCurvesPolygons(std::stack<Entity>& entities) const;
    void UpdateMesh(Entity bezierCurve) const;

    std::vector<float> GenerateBezierPolygonVertices(const C0CurveParameters& params) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const C0CurveParameters& params) const;

    struct RecalculateMeshHandler: EventHandler<Position> {
        RecalculateMeshHandler(Entity bezierCurve, C0CurveSystem& system):
            bezierSystem(system), bezierCurve(bezierCurve) {}

        C0CurveSystem& bezierSystem;
        Entity bezierCurve;

        void HandleEvent(Entity entity, const Position& component, EventType eventType);
    };

    struct ParameterDeletionHandler: EventHandler<C0CurveParameters> {
        ParameterDeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        Coordinator& coordinator;

        void HandleEvent(Entity entity, const C0CurveParameters& component, EventType eventType);
    };
};
