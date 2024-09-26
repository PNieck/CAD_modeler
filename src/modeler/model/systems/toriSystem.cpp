#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <numbers>


void ToriSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ToriSystem>();

    coordinator.RegisterRequiredComponent<ToriSystem, Position>();
    coordinator.RegisterRequiredComponent<ToriSystem, Scale>();
    coordinator.RegisterRequiredComponent<ToriSystem, Rotation>();
    coordinator.RegisterRequiredComponent<ToriSystem, Mesh>();
    coordinator.RegisterRequiredComponent<ToriSystem, TorusParameters>();
}


void ToriSystem::Init(ShaderRepository * shaderRepo)
{
    this->shadersRepo = shaderRepo;
}


Entity ToriSystem::AddTorus(const Position& pos, const TorusParameters& params)
{
    Entity torus = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(torus, pos);

    Scale scale;
    coordinator->AddComponent<Scale>(torus, scale);

    Rotation rot;
    coordinator->AddComponent<Rotation>(torus, rot);

    coordinator->AddComponent<TorusParameters>(torus, params);
    coordinator->AddComponent<Name>(torus, nameGenerator.GenerateName("Torus"));

    auto vertices = GenerateMeshVertices(params);
    auto indices = GenerateMeshIndices(params);

    Mesh mesh;
    mesh.Update(vertices, indices);
    coordinator->AddComponent<Mesh>(torus, mesh);

    return torus;
}


void ToriSystem::SetTorusParameter(Entity entity, const TorusParameters& params)
{
    coordinator->SetComponent<TorusParameters>(entity, params);

    auto vertices = GenerateMeshVertices(params);
    auto indices = GenerateMeshIndices(params);

    Mesh mesh = coordinator->GetComponent<Mesh>(entity);
    mesh.Update(vertices, indices);
    coordinator->SetComponent<Mesh>(entity, mesh);
}


void ToriSystem::Render(const alg::Mat4x4& cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shadersRepo->GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);
        
        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        alg::Mat4x4 modelMtx = position.TranslationMatrix() * rotation.GetRotationMatrix() * scale.ScaleMatrix();
        shader.SetMVP(cameraMtx * modelMtx);
        
        mesh.Use();
        glDrawElements(GL_LINE_LOOP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
        
        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


std::vector<float> ToriSystem::GenerateMeshVertices(const TorusParameters &params) const
{
    float deltaAlpha = 2.f * std::numbers::pi_v<float> / params.meshDensityMinR;
    float deltaBeta = 2.f * std::numbers::pi_v<float> / params.meshDensityMajR;

    std::vector<float> result(params.meshDensityMinR * params.meshDensityMajR * 3);

    for (int actCirc = 0; actCirc < params.meshDensityMajR; actCirc++) {
        float beta = deltaBeta * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < params.meshDensityMinR; act_pt_in_circ++) {
            int index = (actCirc * params.meshDensityMinR + act_pt_in_circ) * 3;

            float alpha = deltaAlpha * act_pt_in_circ;

            result[index] = VertexX(params, alpha, beta);
            result[index + 1] = VertexY(params, alpha);
            result[index + 2] = VertexZ(params, alpha, beta);
        }
    }

    return result;
}


std::vector<uint32_t> ToriSystem::GenerateMeshIndices(const TorusParameters &params) const
{
    std::vector<uint32_t> result(
        params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR + params.meshDensityMinR*params.meshDensityMajR + params.meshDensityMinR
    );

    for (int i = 0; i < params.meshDensityMajR; i++) {
        for (int j=0; j < params.meshDensityMinR; j++) {
            result[i*params.meshDensityMinR + j + i] = j + i*params.meshDensityMinR;
        }

        result[params.meshDensityMinR * (i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    uint32_t index = params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR;

    for (int i = 0; i < params.meshDensityMinR; i++) {
        for (int j=0; j < params.meshDensityMajR; j++) {
            result[index + j + i*params.meshDensityMajR + i] = j * params.meshDensityMinR + i;
        }

        result[index + params.meshDensityMajR*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    return result;
}


float ToriSystem::VertexX(const TorusParameters& params, float alpha, float beta) const
{
    return (params.majorRadius + params.minorRadius * std::cos(alpha)) * std::cos(beta);
}


float ToriSystem::VertexY(const TorusParameters& params, float alpha) const
{
    return params.minorRadius * std::sin(alpha);
}


float ToriSystem::VertexZ(const TorusParameters& params, float alpha, float beta) const
{
    return (params.majorRadius + params.minorRadius * std::cos(alpha)) * std::sin(beta);
}
