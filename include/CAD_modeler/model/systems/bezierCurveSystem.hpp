#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/bezierCurveParameters.hpp"
#include "../components/position.hpp"
#include "utils/nameGenerator.hpp"

#include <vector>
#include <tuple>
#include <stack>


class BezierCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateBezierCurve(const std::vector<Entity>& entities);

    std::vector<float> GenerateBezierPolygonVertices(const BezierCurveParameter& params) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const BezierCurveParameter& params) const;

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static constexpr int CONTROL_POINTS_PER_SEGMENT = 4;

    void RenderCurvesPolygons(std::stack<Entity>& entities) const;

    struct RecalculateMeshHandler: EventHandler<Position> {
        RecalculateMeshHandler(Entity bezierCurve, BezierCurveSystem& system):
            bezierSystem(system), bezierCurve(bezierCurve) {}

        BezierCurveSystem& bezierSystem;
        Entity bezierCurve;

        void operator()(Entity entity, const Position& pos, EventType eventType) const;
        void HandleEvent(Entity entity, const Position& component, EventType eventType);
    };
};
