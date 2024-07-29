#include <CAD_modeler/model/systems/controlNetSystem.hpp>

#include <CAD_modeler/model/components/position.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>


void ControlNetSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<ControlNetSystem>();

    coordinator.RegisterRequiredComponent<ControlNetSystem, ControlNetMesh>();
}


void ControlNetSystem::AddControlPointsNet(Entity entity, const Patches &patches)
{
    ControlNetMesh mesh;
    coordinator->AddComponent<ControlNetMesh>(entity, mesh);

    Update(entity, patches);
}


void ControlNetSystem::Render() const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetStdShader();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (Entity entity: entities) {
        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<ControlNetMesh>(entity);
        mesh.Use();

	    glDrawElements(GL_LINE_STRIP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void ControlNetSystem::Update(Entity entity, const Patches& patches)
{
    coordinator->EditComponent<ControlNetMesh>(entity,
        [this, &patches] (ControlNetMesh& mesh) {
            mesh.Update(
                GenerateVertices(patches),
                GenerateIndices(patches)
            );
        }
    );
}


std::vector<float> ControlNetSystem::GenerateVertices(const Patches &patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsInCol() * patches.PointsInRow() * 3);

    for (int row = 0; row < patches.PointsInRow(); row++) {
        for (int col = 0; col < patches.PointsInCol(); col++) {
            Entity cp = patches.GetPoint(row, col);
            Position pos = coordinator->GetComponent<Position>(cp);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> ControlNetSystem::GenerateIndices(const Patches &patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PointsInCol() * patches.PointsInRow() * 2 + patches.PointsInRow()*2);

    for (int row = 0; row < patches.PointsInRow(); row++) {
        for (int col = 0; col < patches.PointsInCol(); col++) {
            result.push_back(row * patches.PointsInCol() + col);
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    for (int row = 0; row < patches.PointsInCol(); row++) {
        for (int col = 0; col < patches.PointsInRow(); col++) {
            result.push_back(col * patches.PointsInCol() + row);
        }

        result.push_back(std::numeric_limits<uint32_t>::max());
    }

    return result;
}
