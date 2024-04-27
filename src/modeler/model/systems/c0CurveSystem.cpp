#include <CAD_modeler/model/systems/c0CurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/c0CurveParameters.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <cmath>
#include <algorithm>


static constexpr int COORD_IN_VERTEX = 3;


void C0CurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<C0CurveSystem>();

    coordinator.RegisterRequiredComponent<C0CurveSystem, C0CurveParameters>();
    coordinator.RegisterRequiredComponent<C0CurveSystem, Mesh>();
}


Entity C0CurveSystem::CreateBezierCurve(const std::vector<Entity>& entities)
{
    static std::shared_ptr<ParameterDeletionHandler> parameterDeletionHandler(new ParameterDeletionHandler(*coordinator));

    Entity bezierCurve = coordinator->CreateEntity();

    C0CurveParameters params(entities);
    params.drawPolygon = false;

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(params),
        GenerateBezierPolygonIndices(params)
    );

    auto handler = std::make_shared<RecalculateMeshHandler>(bezierCurve, *this);

    for (Entity entity: entities) {
        auto handlerId = coordinator->Subscribe<Position>(entity, std::static_pointer_cast<EventHandler<Position>>(handler));
        params.handlers.insert({ entity, handlerId });
    }

    params.parameterDeletionHandler = coordinator->Subscribe<C0CurveParameters>(bezierCurve, std::static_pointer_cast<EventHandler<C0CurveParameters>>(parameterDeletionHandler));

    coordinator->AddComponent<C0CurveParameters>(bezierCurve, params);
    coordinator->AddComponent<Mesh>(bezierCurve, mesh);
    coordinator->AddComponent<Name>(bezierCurve, nameGenerator.GenerateName("CurveC0_"));

    return bezierCurve;
}


void C0CurveSystem::AddControlPoint(Entity bezierCurve, Entity entity)
{
    coordinator->EditComponent<C0CurveParameters>(bezierCurve,
        [entity, this](C0CurveParameters& params) {
            auto const& controlPoints = params.ControlPoints();
            Entity controlPoint = *controlPoints.begin();
            HandlerId handlerId = params.handlers.at(controlPoint);
            auto eventHandler = coordinator->GetEventHandler<Position>(controlPoint, handlerId);

            params.AddControlPoint(entity);
            params.handlers.insert({entity, coordinator->Subscribe<Position>(entity, eventHandler)});
        }
    );

    UpdateMesh(bezierCurve);
}


void C0CurveSystem::DeleteControlPoint(Entity bezierCurve, Entity entity)
{
    bool entityDeleted = false;

    coordinator->EditComponent<C0CurveParameters>(bezierCurve,
        [&entityDeleted, bezierCurve, entity, this](C0CurveParameters& params) {
            params.DeleteControlPoint(entity);
            coordinator->Unsubscribe<Position>(entity, params.handlers.at(entity));
            params.handlers.erase(entity);

            if (params.handlers.size() == 0) {
                coordinator->DestroyEntity(bezierCurve);
                entityDeleted = true;
            }
        }
    );

    if (!entityDeleted)
        UpdateMesh(bezierCurve);
}


void C0CurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetBezierShader();
    std::stack<Entity> polygonsToDraw;

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

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
        RenderCurvesPolygons(polygonsToDraw);
}


void C0CurveSystem::RenderCurvesPolygons(std::stack<Entity>& entities) const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    while (!entities.empty()) {
        Entity entity = entities.top();
        entities.pop();

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


void C0CurveSystem::UpdateMesh(Entity bezierCurve) const
{
    coordinator->EditComponent<Mesh>(bezierCurve,
        [bezierCurve, this](Mesh& mesh) {
            auto const& params = coordinator->GetComponent<C0CurveParameters>(bezierCurve);

            mesh.Update(
                GenerateBezierPolygonVertices(params),
                GenerateBezierPolygonIndices(params)
            );
        }
    );
}


std::vector<float> C0CurveSystem::GenerateBezierPolygonVertices(const C0CurveParameters& params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<float> result(controlPoints.size() * COORD_IN_VERTEX);

    int i = 0;
    for (Entity entity: controlPoints) {
        auto const& pos = coordinator->GetComponent<Position>(entity);

        result[i * COORD_IN_VERTEX] = pos.GetX();
        result[i * COORD_IN_VERTEX + 1] = pos.GetY();
        result[i * COORD_IN_VERTEX + 2] = pos.GetZ();

        i++;
    }

    return result;
}


int ceiling(int x, int y) {
    return (x + y - 1) / y;
}


std::vector<uint32_t> C0CurveSystem::GenerateBezierPolygonIndices(const C0CurveParameters& params) const
{
    auto const& controlPoints = params.ControlPoints();

    int fullSegmentsCnt = (controlPoints.size() - 1) / (CONTROL_POINTS_PER_SEGMENT - 1);
    int allSegmentsCnt = ceiling(controlPoints.size() - 1, CONTROL_POINTS_PER_SEGMENT - 1);
    int resultSize = allSegmentsCnt * CONTROL_POINTS_PER_SEGMENT;

    std::vector<uint32_t> result;
    result.reserve(resultSize);

    // Add all full segments
    int firstControlPoint = 0;
    for (; firstControlPoint < fullSegmentsCnt * CONTROL_POINTS_PER_SEGMENT - 3; firstControlPoint += 3) {
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

    // Repet the last control point
    while (result.size() < resultSize) {
        result.push_back(controlPoints.size() - 1);
    }
    

    return result;
}


void C0CurveSystem::RecalculateMeshHandler::HandleEvent(Entity entity, const Position& pos, EventType eventType)
{
    bool curveDestroyed = false;

    if (eventType == EventType::ComponentDeleted) {
        bezierSystem.coordinator->EditComponent<C0CurveParameters>(bezierCurve,
            [&curveDestroyed, entity, this](C0CurveParameters& params) {
                params.DeleteControlPoint(entity);
                bezierSystem.coordinator->Unsubscribe<Position>(entity, params.handlers.at(entity));
                params.handlers.erase(entity);

                if (params.handlers.size() == 0) {
                    bezierSystem.coordinator->DestroyEntity(bezierCurve);
                    curveDestroyed = true;
                }
            }
        );
    }

    if (curveDestroyed)
        return;

    bezierSystem.UpdateMesh(bezierCurve);
}


void C0CurveSystem::ParameterDeletionHandler::HandleEvent(Entity entity, const C0CurveParameters& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto entitiesIt = component.ControlPoints().begin();
    auto handlersIt = component.handlers.begin();

    while (handlersIt != component.handlers.end() || entitiesIt != component.ControlPoints().end()) {
        coordinator.Unsubscribe<Position>(*entitiesIt, (*handlersIt).second);

        ++entitiesIt;
        ++handlersIt;
    }

    coordinator.Unsubscribe<C0CurveParameters>(entity, component.parameterDeletionHandler);
}
