#include "CAD_modeler/model/systems/c2PatchesRenderSystem.hpp"

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/drawStd.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"

#include <ecs/coordinator.hpp>

void C2PatchesRenderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2PatchesRenderSystem>();

    coordinator.RegisterComponent<DrawStd>();

    coordinator.RegisterRequiredComponent<C2PatchesRenderSystem, C2Patches>();
    coordinator.RegisterRequiredComponent<C2PatchesRenderSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C2PatchesRenderSystem, Mesh>();
    coordinator.RegisterRequiredComponent<C2PatchesRenderSystem, DrawStd>();
}


void C2PatchesRenderSystem::Render(const alg::Mat4x4 &cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBSplineSurfaceShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

        glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}
