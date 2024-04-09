#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/bezierCurveParameters.hpp"

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

    void PointChangedPosition(Entity entity, const Position& pos);
};
