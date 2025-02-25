#include <CAD_modeler/model/systems/c0CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/c0CurveParameters.hpp>
#include <CAD_modeler/model/components/curveControlPoints.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/toUpdate.hpp>
#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <cmath>
#include <algorithm>
#include <cassert>


void C0CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C0CurveSystem>();

    coordinator.RegisterRequiredComponent<C0CurveSystem, C0CurveParameters>();
    coordinator.RegisterRequiredComponent<C0CurveSystem, CurveControlPoints>();
    coordinator.RegisterRequiredComponent<C0CurveSystem, Mesh>();
}


Entity C0CurveSystem::CreateC0Curve(const std::vector<Entity>& cps)
{
    Entity curve = coordinator->GetSystem<CurveControlPointsSystem>()->CreateControlPoints(cps, Coordinator::GetSystemID<C0CurveSystem>());

    C0CurveParameters params;

    auto const& controlPoints = coordinator->GetComponent<CurveControlPoints>(curve);

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(controlPoints),
        GenerateBezierPolygonIndices(controlPoints)
    );

    coordinator->AddComponent<C0CurveParameters>(curve, params);
    coordinator->AddComponent<Mesh>(curve, mesh);

    return curve;
}


void C0CurveSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierCurveShader();
    std::stack<Entity> polygonsToDraw;

    UpdateEntities();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<C0CurveParameters>(entity);

        if (params.drawPolygon)
            polygonsToDraw.push(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 4);
	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }

    if (!polygonsToDraw.empty())
        RenderCurvesPolygons(polygonsToDraw, cameraMtx);
}


Position CalculatePositionInSinglePatch(const Position& cp1, const Position& cp2, const Position& cp3, const Position& cp4, float t)
{
    assert(t >= 0.0f);
    assert(t <= 1.0f);

    const float oneMinusT = 1 - t;

    // Bernstein polynomials
    const float b0 = oneMinusT * oneMinusT * oneMinusT;
    const float b1 = 3 * oneMinusT * oneMinusT * t;
    const float b2 = 3 * oneMinusT * t * t;
    const float b3 = t * t * t;

    const alg::Vec3 result = cp1.vec * b0 +
                             cp2.vec * b1 +
                             cp3.vec * b2 +
                             cp4.vec * b3;

    return { result };
}


Position C0CurveSystem::CalculatePosition(const std::vector<Position>& cpPositions, float t)
{
    float tFloor = std::floor(t);
    int firstIdx = static_cast<int>(tFloor);

    // Normalizing t so it is between 0 and 1
    t -= tFloor;

    return CalculatePositionInSinglePatch(
        cpPositions[firstIdx],
        cpPositions[firstIdx + 1],
        cpPositions[firstIdx + 2],
        cpPositions[firstIdx + 3],
        t
    );
}


Position C0CurveSystem::CalculatePosition(const std::vector<Entity>& cps, float t) const
{
    float tFloor = std::floor(t);
    int firstIdx = static_cast<int>(tFloor);

    // Normalizing t so it is between 0 and 1
    t -= tFloor;

    return CalculatePositionInSinglePatch(
        coordinator->GetComponent<Position>(cps[firstIdx]),
        coordinator->GetComponent<Position>(cps[firstIdx + 1]),
        coordinator->GetComponent<Position>(cps[firstIdx + 2]),
        coordinator->GetComponent<Position>(cps[firstIdx + 3]),
        t
    );
}


void C0CurveSystem::RenderCurvesPolygons(std::stack<Entity>& cps, const alg::Mat4x4& cameraMtx) const
{
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!cps.empty()) {
        Entity entity = cps.top();
        cps.pop();

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C0CurveSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        auto const& cps = coordinator->GetComponent<CurveControlPoints>(entity);

        if (cps.Size() != 0) {
            UpdateMesh(entity, cps);
            toUpdateSystem->Unmark(entity);
        }
        else {
            coordinator->DestroyEntity(entity);
        }
    }
}


void C0CurveSystem::UpdateMesh(Entity curve, const CurveControlPoints& cps) const
{
    coordinator->EditComponent<Mesh>(curve,
        [&cps, curve, this](Mesh& mesh) {
            mesh.Update(
                GenerateBezierPolygonVertices(cps),
                GenerateBezierPolygonIndices(cps)
            );
        }
    );
}


std::vector<float> C0CurveSystem::GenerateBezierPolygonVertices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.GetPoints();

    std::vector<float> result;
    result.reserve(controlPoints.size() * alg::Vec3::dim);

    for (Entity entity: controlPoints) {
        auto const& pos = coordinator->GetComponent<Position>(entity);

        result.push_back(pos.GetX());
        result.push_back(pos.GetY());
        result.push_back(pos.GetZ());
    }

    return result;
}


inline size_t ceiling(size_t x, size_t y) {
    return (x + y - 1) / y;
}


std::vector<uint32_t> C0CurveSystem::GenerateBezierPolygonIndices(const CurveControlPoints& params) const
{
    auto const& controlPoints = params.GetPoints();

    size_t fullSegmentsCnt = (controlPoints.size() - 1) / (CONTROL_POINTS_PER_SEGMENT - 1);
    size_t allSegmentsCnt = ceiling(controlPoints.size() - 1, CONTROL_POINTS_PER_SEGMENT - 1);
    size_t resultSize = allSegmentsCnt * CONTROL_POINTS_PER_SEGMENT;

    std::vector<uint32_t> result;
    result.reserve(resultSize);

    // Add all full segments
    int firstControlPoint = 0;
    for (; firstControlPoint < static_cast<int>(fullSegmentsCnt * CONTROL_POINTS_PER_SEGMENT) - 3; firstControlPoint += 3) {
        result.push_back(firstControlPoint);
        result.push_back(firstControlPoint + 1);
        result.push_back(firstControlPoint + 2);
        result.push_back(firstControlPoint + 3);
    }

    // Add the rest of control points
    int i = firstControlPoint;
    while (i < controlPoints.size()) {
        result.push_back(i++);
    }

    // Repeat the last control point
    while (result.size() < resultSize)
        result.push_back(
            static_cast<uint32_t>(controlPoints.size() - 1)
        );

    return result;
}
