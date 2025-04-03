#pragma once

#include <ecs/system.hpp>

#include "../components/curveControlPoints.hpp"
#include "../components/position.hpp"
#include "curveControlPointsSystem.hpp"

#include <vector>
#include <stack>


class C0CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateC0Curve(const std::vector<Entity>& entities);
    Entity CreateC0Curve(const Entity entity)
        { return CreateC0Curve(std::vector{entity}); }

    void AddControlPoint(const Entity bezierCurve, const Entity entity) const
        { coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity, Coordinator::GetSystemID<C0CurveSystem>()); }

    void DeleteControlPoint(const Entity bezierCurve, const Entity entity) const
        { coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity, Coordinator::GetSystemID<C0CurveSystem>()); }

    void MergeControlPoints(const Entity curve, const Entity oldCP, const Entity newCP) const
        { coordinator->GetSystem<CurveControlPointsSystem>()->MergeControlPoints(curve, oldCP, newCP, Coordinator::GetSystemID<C0CurveSystem>()); }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

    static Position CalculatePosition(const std::vector<Position>& cpPositions, float t);
    Position CalculatePosition(const std::vector<Entity>& cps, float t) const;

private:
    static constexpr int CONTROL_POINTS_PER_SEGMENT = 4;

    void RenderCurvesPolygons(std::stack<Entity>& entities, const alg::Mat4x4& cameraMtx) const;
    void UpdateMesh(Entity curve, const CurveControlPoints& cps) const;

    std::vector<float> GenerateBezierPolygonVertices(const CurveControlPoints& cps) const;
    std::vector<uint32_t> GenerateBezierPolygonIndices(const CurveControlPoints& cps) const;
};
