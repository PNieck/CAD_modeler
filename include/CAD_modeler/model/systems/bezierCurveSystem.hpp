#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/bezierCurveParameters.hpp"
#include "../components/position.hpp"

#include <vector>


class BezierCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateBezierCurve(const std::vector<Entity>& entities);

    void Render() const;

private:
    ShaderRepository* shaderRepo;

    std::vector<float> GenerateBezierPolygonVertices(const BezierCurveParameter& params) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const BezierCurveParameter& params) const;

    struct ControlPointChangedPositionCallback {
        ControlPointChangedPositionCallback(Entity bezierCurve, BezierCurveSystem& system):
            bezierSystem(system), bezierCurve(bezierCurve) {}

        BezierCurveSystem& bezierSystem;
        Entity bezierCurve;

        void operator()(Entity entity, const Position& pos) const;
    };
};
