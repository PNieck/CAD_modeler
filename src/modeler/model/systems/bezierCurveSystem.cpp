#include <CAD_modeler/model/systems/bezierCurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/bezierCurveParameters.hpp>
#include <CAD_modeler/model/components/bezierMesh.hpp>
#include <CAD_modeler/model/components/position.hpp>
#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <cmath>


static constexpr int COORD_IN_VERTEX = 3;


void BezierCurveSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<BezierCurveSystem>();

    coordinator.RegisterRequiredComponent<BezierCurveSystem, BezierCurveParameter>();
    coordinator.RegisterRequiredComponent<BezierCurveSystem, Mesh>();
    coordinator.RegisterRequiredComponent<BezierCurveSystem, BezierMesh>();
}


Entity BezierCurveSystem::CreateBezierCurve(const std::vector<Entity>& entities)
{
    Entity bezierCurve = coordinator->CreateEntity();

    BezierCurveParameter params(entities);
    params.drawPolygon = true;

    Mesh mesh;
    mesh.Update(
        GenerateBezierPolygonVertices(params),
        GenerateBezierPolygonIndices(params)
    );

    BezierMesh bezierMesh;
    bezierMesh.Update(
        CalculateBezierMesh(params),
        CalculateBezierIndices(params)
    );
    
    ControlPointChangedPositionCallback callback(bezierCurve, *this);

    for (Entity entity: entities) {
        auto handlerId = coordinator->SubscribeToComponentChange<Position>(entity, callback);
        params.handlers.insert({ entity, handlerId });
    }

    coordinator->AddComponent<BezierCurveParameter>(bezierCurve, params);
    coordinator->AddComponent<Mesh>(bezierCurve, mesh);
    coordinator->AddComponent<BezierMesh>(bezierCurve, bezierMesh);

    return bezierCurve;
}


void BezierCurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    glm::mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(glm::vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& params = coordinator->GetComponent<BezierCurveParameter>(entity);

        if (params.drawPolygon) {
            auto const& mesh = coordinator->GetComponent<Mesh>(entity);

            bool selection = selectionSystem->IsSelected(entity);

            if (selection)
                shader.SetColor(glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

            mesh.Use();
            glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

            if (selection)
                shader.SetColor(glm::vec4(1.0f));
        }

        auto const& bezierMesh = coordinator->GetComponent<BezierMesh>(entity);

        bezierMesh.Use();
        glDrawElements(GL_LINE_STRIP, bezierMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    }
}


std::vector<float> BezierCurveSystem::GenerateBezierPolygonVertices(const BezierCurveParameter& params) const
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


std::vector<uint32_t> BezierCurveSystem::GenerateBezierPolygonIndices(const BezierCurveParameter& params) const
{
    auto const& controlPoints = params.ControlPoints();

    std::vector<uint32_t> result(controlPoints.size());

    for (int i=0; i < controlPoints.size(); i++) {
        result[i] = i;
    }

    return result;
}


std::vector<float> BezierCurveSystem::CalculateBezierMesh(const BezierCurveParameter & params) const
{
    static constexpr int segments = 50;

    std::vector<float> result(segments * 3);
    float t = 0.0f;

    auto const& controlPoints = params.ControlPoints();

    auto const& pos0 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& pos1 = coordinator->GetComponent<Position>(controlPoints[1]);
    auto const& pos2 = coordinator->GetComponent<Position>(controlPoints[2]);
    auto const& pos3 = coordinator->GetComponent<Position>(controlPoints[3]);

    for (int i=0; i < segments; i++) {
        glm::vec3 pos = std::powf(1-t, 3) * pos0.vec +
            3 * std::powf(1-t, 2) * t * pos1.vec +
            3 * (1-t) * std::powf(t, 2) * pos2.vec +
            std::powf(t, 3) * pos3.vec;

        result[3*i] = pos.x;
        result[3*i + 1] = pos.y;
        result[3*i + 2] = pos.z;

        t += 1.0f/static_cast<float>(segments - 1);
    }

    return result;
}


std::vector<uint32_t> BezierCurveSystem::CalculateBezierIndices(const BezierCurveParameter & params) const
{
    std::vector<uint32_t> result(50);

    for (int i=0; i < 50; i++) {
        result[i] = i;
    }

    return result;
}


void BezierCurveSystem::ControlPointChangedPositionCallback::operator()(Entity entity, const Position & pos) const
{
    auto mesh = bezierSystem.coordinator->GetComponent<Mesh>(bezierCurve);
    auto bezierMesh = bezierSystem.coordinator->GetComponent<BezierMesh>(bezierCurve);
    auto const& params = bezierSystem.coordinator->GetComponent<BezierCurveParameter>(bezierCurve);

    mesh.Update(
        bezierSystem.GenerateBezierPolygonVertices(params),
        bezierSystem.GenerateBezierPolygonIndices(params)
    );

    bezierMesh.Update(
        bezierSystem.CalculateBezierMesh(params),
        bezierSystem.CalculateBezierIndices(params)
    );

    bezierSystem.coordinator->SetComponent<Mesh>(bezierCurve, mesh);
}
