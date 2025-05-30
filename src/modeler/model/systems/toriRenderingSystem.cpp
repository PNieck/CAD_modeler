#include <CAD_modeler/model/systems/toriRenderingSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>


void ToriRenderingSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<ToriRenderingSystem>();
}


void ToriRenderingSystem::AddEntity(const Entity e)
{
    coordinator->AddComponent(e, Mesh());
    Update(e);

    entities.insert(e);
}


void ToriRenderingSystem::RemoveEntity(const Entity e)
{
    coordinator->DeleteComponent<Mesh>(e);
    entities.erase(e);
}


void ToriRenderingSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);

        const bool selection = selectionSystem->IsSelected(entity);

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


void ToriRenderingSystem::Update(const Entity e)
{
    const auto& params = coordinator->GetComponent<TorusParameters>(e);

    coordinator->EditComponent<Mesh>(e,
        [this, &params] (Mesh& mesh) {
            mesh.Update(
                GenerateMeshVertices(params),
                GenerateMeshIndices(params)
            );
        }
    );
}


std::vector<float> ToriRenderingSystem::GenerateMeshVertices(const TorusParameters &params)
{
    const float deltaU = 2.f * std::numbers::pi_v<float> / params.meshDensityMinR;
    const float deltaV = 2.f * std::numbers::pi_v<float> / params.meshDensityMajR;

    std::vector<float> result(params.meshDensityMinR * params.meshDensityMajR * 3);

    for (int actCirc = 0; actCirc < params.meshDensityMajR; actCirc++) {
        const float v = deltaV * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < params.meshDensityMinR; act_pt_in_circ++) {
            const int index = (actCirc * params.meshDensityMinR + act_pt_in_circ) * 3;

            const float u = deltaU * act_pt_in_circ;

            auto point = ToriSystem::PointOnSurface(params, u, v);

            result[index] = point.X();
            result[index + 1] = point.Y();
            result[index + 2] = point.Z();
        }
    }

    return result;
}


std::vector<uint32_t> ToriRenderingSystem::GenerateMeshIndices(const TorusParameters &params)
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

    const uint32_t index = params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR;

    for (int i = 0; i < params.meshDensityMinR; i++) {
        for (int j=0; j < params.meshDensityMajR; j++) {
            result[index + j + i*params.meshDensityMajR + i] = j * params.meshDensityMinR + i;
        }

        result[index + params.meshDensityMajR*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    return result;
}

