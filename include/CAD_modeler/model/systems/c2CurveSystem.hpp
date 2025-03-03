#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>
#include <ecs/eventHandler.hpp>

#include "../components/c2CurveParameters.hpp"
#include "../components/curveControlPoints.hpp"
#include "../components/bezierControlPoints.hpp"
#include "curveControlPointsSystem.hpp"

#include <vector>


class C2CurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateC2Curve(const std::vector<Entity>& controlPoints);
    inline Entity CreateC2Curve(Entity controlPoint)
        { CreateC2Curve({controlPoint}); }

    inline void AddControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity, Coordinator::GetSystemID<C2CurveSystem>()); }

    inline void DeleteControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity, Coordinator::GetSystemID<C2CurveSystem>()); }

    inline void MergeControlPoints(Entity curve, Entity oldCP, Entity newCP)
        { coordinator->GetSystem<CurveControlPointsSystem>()->MergeControlPoints(curve, oldCP, newCP, Coordinator::GetSystemID<C2CurveSystem>()); }

    void ShowBSplinePolygon(Entity entity);
    void HideBSplinePolygon(Entity entity);

    void ShowBezierPolygon(Entity entity);
    void HideBezierPolygon(Entity entity);

    void ShowBezierControlPoints(Entity entity);
    void HideBezierControlPoints(Entity entity);

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    static constexpr int MIN_CTRL_PTS_CNT = 4;

    void UpdateEntities() const;
    void UpdateCurveMesh(Entity curve, const CurveControlPoints& cps) const;
    void UpdateBSplinePolygon(Entity curve) const;
    void UpdateBezierControlPoints(Entity curve) const;
    void UpdateBezierCtrlPtsHandlers(Entity curve, BezierControlPoints& cps) const;

    void RenderBSplinePolygons(std::stack<Entity>& entities, const alg::Mat4x4& cameraMtx) const;
    void RenderBezierPolygons(std::stack<Entity>& entities, const alg::Mat4x4& cameraMtx) const;

    BezierControlPoints CreateBezierControlPoints(const CurveControlPoints& cps) const;
    void DeleteBezierControlPoints(Entity entity) const;
    std::vector<alg::Vec3> CreateBezierControlPointsPositions(const CurveControlPoints& cps) const;

    std::vector<float> GenerateCurveMeshVertices(const CurveControlPoints& cps) const;
    std::vector<uint32_t> GenerateCurveMeshIndices(const CurveControlPoints& cps) const;

    std::vector<float> GenerateBSplinePolygonVertices(const CurveControlPoints& cps) const;
    std::vector<uint32_t> GenerateBSplinePolygonIndices(const CurveControlPoints& cps) const;

    inline size_t BezierControlPointsCnt(size_t bSplineCtrlPts) const
        { return bSplineCtrlPts < MIN_CTRL_PTS_CNT ? 0 : 4 + 3 * (bSplineCtrlPts - 4); }


    class BezierCtrlPtMovedHandler: public EventHandler<Position> {
    public:
        BezierCtrlPtMovedHandler(Coordinator& coordinator, Entity c2Curve):
            coordinator(coordinator), c2Curve(c2Curve) {}

        void HandleEvent(Entity entity, const Position& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
        Entity c2Curve;

        void FirstCPMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs) const;
        void LastCPMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs) const;

        void FirstFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
        void SecondFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
        void ThirdFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
    };


    // FIXME: delete code repetition
    class DeletionHandler: public EventHandler<BezierControlPoints> {
    public:
        DeletionHandler(Coordinator& coordinator):
            coordinator(coordinator) {}

        void HandleEvent(Entity entity, const BezierControlPoints& component, EventType eventType) override;

    private:
        Coordinator& coordinator;
    };
};
