#include "CAD_modeler/model/systems/trimmedC2PatchesRenderSystem.hpp"

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/drawTrimmed.hpp"
#include "CAD_modeler/model/components/uvVisualization.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"

#include <ecs/coordinator.hpp>


void TrimmedC2PatchesRenderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<TrimmedC2PatchesRenderSystem>();

    coordinator.RegisterComponent<DrawTrimmed>();

    coordinator.RegisterRequiredComponent<TrimmedC2PatchesRenderSystem, C2Patches>();
    coordinator.RegisterRequiredComponent<TrimmedC2PatchesRenderSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<TrimmedC2PatchesRenderSystem, Mesh>();
    coordinator.RegisterRequiredComponent<TrimmedC2PatchesRenderSystem, DrawTrimmed>();
    coordinator.RegisterRequiredComponent<TrimmedC2PatchesRenderSystem, UvVisualization>();
}


void TrimmedC2PatchesRenderSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetTrimmedBSplineSurfaceShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        auto const& uv = coordinator->GetComponent<UvVisualization>(entity);
        auto const& patches = coordinator->GetComponent<C2Patches>(entity);

        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        shader.SetSurfaceSize(patches.PatchesInRow(), patches.PatchesInCol());

        mesh.Use();
        uv.UseTexture();

        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

        glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}
