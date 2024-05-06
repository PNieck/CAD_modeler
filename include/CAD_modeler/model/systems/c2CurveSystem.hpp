#pragma once

#include <ecs/system.hpp>
#include <ecs/coordinator.hpp>
#include <ecs/eventHandler.hpp>

#include "shaders/shaderRepository.hpp"
#include "../components/c2CurveParameters.hpp"
#include "../components/curveControlPoints.hpp"
#include "../components/bezierControlPoints.hpp"
#include "utils/nameGenerator.hpp"
#include "curveControlPointsSystem.hpp"

#include <vector>


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

    void ShowBSplinePolygon(Entity entity);
    void HideBSplinePolygon(Entity entity);

    void ShowBezierPolygon(Entity entity);
    void HideBezierPolygon(Entity entity);

    void ShowBezierControlPoints(Entity entity);
    void HideBezierControlPoints(Entity entity);

    void Render() const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    static constexpr int MIN_CTRL_PTS_CNT = 4;

    void UpdateEntities() const;
    void UpdateCurveMesh(Entity curve) const;
    void UpdateBSplinePolygon(Entity curve) const;
    void UpdateBezierControlPoints(Entity curve, const C2CurveParameters& params) const;
    void UpdateBezierCtrlPtsHandlers(Entity curve, BezierControlPoints& ctrlPts) const;

    void RenderBSplinePolygons(std::stack<Entity>& entities) const;
    void RenderBezierPolygons(std::stack<Entity>& entities) const;

    BezierControlPoints CreateBezierControlPoints(const CurveControlPoints& params) const;
    void DeleteBezierControlPoints(Entity entity) const;
    std::vector<alg::Vec3> CreateBezierControlPointsPositions(const CurveControlPoints& params) const;

    std::vector<float> GenerateCurveMeshVertices(const CurveControlPoints& params) const;
    std::vector<uint32_t> GenerateCurveMeshIndices(const CurveControlPoints& params) const;

    std::vector<float> GenerateBSplinePolygonVertices(const CurveControlPoints& params) const;
    std::vector<uint32_t> GenerateBSplinePolygonIndices(const CurveControlPoints& params) const;

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
        void FirstFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
        void SecondFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
        void ThirdFromFullLineMoved(const std::vector<Entity>& bezierCPs, const std::vector<Entity>& bSplineCPs, int bezierCtrlPtIndex) const;
    };
};
