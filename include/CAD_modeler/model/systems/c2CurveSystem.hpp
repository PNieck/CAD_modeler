#pragma once

#include <ecs/system.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c2CurveParameters.hpp"
#include "../components/curveControlPoints.hpp"
#include "utils/nameGenerator.hpp"


class C2CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateC2Curve(const std::vector<Entity>& entities);
    inline Entity CreateC2Curve(Entity entity)
        { CreateC2Curve({entity}); }

    void AddControlPoint(Entity bezierCurve, Entity entity);
    void DeleteControlPoint(Entity bezierCurve, Entity entity);

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    std::vector<float> GenerateBezierPolygonVertices(const CurveControlPoints& params) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const CurveControlPoints& params) const;

};
