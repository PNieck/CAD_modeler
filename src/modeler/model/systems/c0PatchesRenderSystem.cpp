#include "CAD_modeler/model/systems/c0PatchesRenderSystem.hpp"

#include "CAD_modeler/model/components/c0Patches.hpp"
#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/drawStd.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"

#include <ecs/coordinator.hpp>

void C0PatchesRenderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesRenderSystem>();

    coordinator.RegisterComponent<DrawStd>();

    coordinator.RegisterRequiredComponent<C0PatchesRenderSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0PatchesRenderSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0PatchesRenderSystem, Mesh>();
    coordinator.RegisterRequiredComponent<C0PatchesRenderSystem, DrawStd>();
}


void C0PatchesRenderSystem::Render(const alg::Mat4x4 &cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBezierSurfaceShader();

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

        glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}



