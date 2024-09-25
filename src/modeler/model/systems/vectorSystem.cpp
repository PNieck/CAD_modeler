#include <CAD_modeler/model/systems/vectorSystem.hpp>

#include <CAD_modeler/model/components/vectorMesh.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <ecs/coordinator.hpp>


void VectorSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<VectorSystem>();

    coordinator.RegisterRequiredComponent<VectorSystem, Position>();
    coordinator.RegisterRequiredComponent<VectorSystem, Vector>();
    coordinator.RegisterRequiredComponent<VectorSystem, VectorMesh>();
}


Entity VectorSystem::AddVector(const alg::Vec3& vec, const Position& pos)
{
    Entity entity = coordinator->CreateEntity();
    AddVector(entity, vec, pos);

    return entity;
}


void VectorSystem::AddVector(Entity entity, const alg::Vec3 &vec, const Position &pos)
{
    coordinator->AddComponent<VectorMesh>(entity, VectorMesh());
    coordinator->AddComponent<Position>(entity, pos);
    coordinator->AddComponent<Vector>(entity, vec);

    UpdateMesh(entity);
}


void VectorSystem::AddVector(Entity entity, const alg::Vec3 &vec)
{
    if (!coordinator->HasComponent<Position>(entity))
        coordinator->AddComponent<Position>(entity, Position(0.0f));

    coordinator->AddComponent<VectorMesh>(entity, VectorMesh());
    coordinator->AddComponent<Vector>(entity, vec);

    UpdateMesh(entity);
}


void VectorSystem::Render(const alg::Mat4x4 &cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto entity: entities) {
        bool selection = selectionSystem->IsSelected(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        shader.SetMVP(cameraMtx * position.TranslationMatrix());

        auto const& mesh = coordinator->GetComponent<VectorMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void VectorSystem::UpdateMesh(Entity entity)
{
    coordinator->EditComponent<VectorMesh>(entity,
        [this, entity](VectorMesh& mesh) {
            const auto& vec = coordinator->GetComponent<Vector>(entity);

            mesh.Update(
                GenerateNetVertices(vec),
                GenerateNetIndices()
            );
        }
    );
}


std::vector<float> VectorSystem::GenerateNetVertices(const Vector &vec) const
{
    std::vector<float> result;
    result.reserve(12);

    result.push_back(0.0f);
    result.push_back(0.0f);
    result.push_back(0.0f);

    result.push_back(vec.X());
    result.push_back(vec.Y());
    result.push_back(vec.Z());

    alg::Vec3 tmp1 = (-vec).Normalize() * std::min(0.5f, 0.2f * vec.Length()) + vec;

    alg::Vec3 tmp2 = tmp1 + alg::Vec3(0.0f, 0.2f, 0.0f);
    alg::Vec3 tmp3 = tmp1 - alg::Vec3(0.0f, 0.2f, 0.0f);

    result.push_back(tmp2.X());
    result.push_back(tmp2.Y());
    result.push_back(tmp2.Z());

    result.push_back(tmp3.X());
    result.push_back(tmp3.Y());
    result.push_back(tmp3.Z());

    return result;
}


std::vector<uint32_t> VectorSystem::GenerateNetIndices() const
{
    return {
        0, 1, 2, 1, 3
    };
}
