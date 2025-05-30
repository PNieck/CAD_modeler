#include <CAD_modeler/model/systems/trimmedToriRenderingSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/meshWithUV.hpp>
#include <CAD_modeler/model/components/uvVisualization.hpp>

#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include "CAD_modeler/model/systems/utils/getSurfaceSystem.hpp"


void TrimmedToriRenderingSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<TrimmedToriRenderingSystem>();

    coordinator.RegisterComponent<MeshWithUV>();
}


void TrimmedToriRenderingSystem::AddEntity(const Entity e)
{
    coordinator->AddComponent(e, MeshWithUV());
    Update(e);

    entities.insert(e);
}


void TrimmedToriRenderingSystem::RemoveEntity(const Entity e)
{
    coordinator->DeleteComponent<MeshWithUV>(e);
    entities.erase(e);
}


void TrimmedToriRenderingSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetTrimmedTorusShader();
    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<MeshWithUV>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);
        auto const& uv = coordinator->GetComponent<UvVisualization>(entity);

        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        alg::Mat4x4 modelMtx = position.TranslationMatrix() * rotation.GetRotationMatrix() * scale.ScaleMatrix();
        shader.SetMVP(cameraMtx * modelMtx);

        mesh.Use();
        uv.UseTexture();
        glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void TrimmedToriRenderingSystem::Update(const Entity e)
{
    const auto& params = coordinator->GetComponent<TorusParameters>(e);

    coordinator->EditComponent<MeshWithUV>(e,
        [this, &params, e] (MeshWithUV& mesh) {
            mesh.Update(
                GenerateMeshVertices(e, params),
                GenerateMeshIndices(params)
            );
        }
    );
}


std::vector<float> TrimmedToriRenderingSystem::GenerateMeshVertices(Entity e, const TorusParameters &params) const
{
    constexpr float maxU = ToriSystem::MaxU();
    constexpr float maxV = ToriSystem::MaxV();

    const float deltaU = 2.f * std::numbers::pi_v<float> / params.meshDensityMinR;
    const float deltaV = 2.f * std::numbers::pi_v<float> / params.meshDensityMajR;

    // TODO: fix reserve number
    std::vector<float> result;
    result.reserve(params.meshDensityMinR * params.meshDensityMajR * 5);

    for (int actCirc = 0; actCirc <= params.meshDensityMajR; actCirc++) {
        const float v = deltaV * static_cast<float>(actCirc);

        for (int act_pt_in_circ = 0; act_pt_in_circ <= params.meshDensityMinR; act_pt_in_circ++) {
            const float u = deltaU * static_cast<float>(act_pt_in_circ);

            auto point = ToriSystem::PointOnSurface(params, u, v);

            result.push_back(point.X());
            result.push_back(point.Y());
            result.push_back(point.Z());
            result.push_back(u/maxU);
            result.push_back(v/maxV);
        }
    }

    return result;
}


std::vector<uint32_t> TrimmedToriRenderingSystem::GenerateMeshIndices(const TorusParameters &params)
{
    // std::vector<uint32_t> result(
    //     params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR + params.meshDensityMinR*params.meshDensityMajR + params.meshDensityMinR
    // );

    // TODO: fix reserve number
    std::vector<uint32_t> result;
    result.reserve(params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR + params.meshDensityMinR*params.meshDensityMajR + params.meshDensityMinR);

    for (int i = 0; i < params.meshDensityMajR; i++) {
        for (int j=0; j <= params.meshDensityMinR; j++) {
            result.push_back(j + i*(params.meshDensityMinR+1));
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    for (int i = 0; i < params.meshDensityMinR; i++) {
        for (int j=0; j <= params.meshDensityMajR; j++) {
            result.push_back(j * (params.meshDensityMinR + 1) + i);
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    return result;
}
