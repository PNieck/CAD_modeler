#include <CAD_modeler/model/systems/gridSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/components/mesh.hpp>


void GridSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<GridSystem>();

    coordinator.RegisterComponent<Mesh>();
}


GridSystem::GridSystem():
    gridShader("../../shaders/grid.vert", "../../shaders/grid.frag")
{
}


void GridSystem::Init()
{
    grid = coordinator->CreateEntity();

    Mesh mesh;
    std::vector<float> vertices = {
         1.0f,  1.0f, 0.0f,  //
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    std::vector<uint32_t> indices = {
        0, 1, 3, // First triangle
        1, 2, 3  // Second triangle
    };

    mesh.Update(vertices, indices);

    coordinator->AddComponent<Mesh>(grid, mesh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void GridSystem::Render() const
{
    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& gridMesh = coordinator->GetComponent<Mesh>(grid);

    glm::mat4x4 viewMtx = cameraSystem->ViewMatrix();
    glm::mat4x4 projectionMtx = cameraSystem->PerspectiveMatrix();

    gridShader.use();
    gridShader.setFloat("far", cameraSystem->GetFarPlane());
    gridShader.setFloat("near", cameraSystem->GetNearPlane());
    gridShader.setMatrix4("view", viewMtx);
    gridShader.setMatrix4("proj", projectionMtx);
    
    gridMesh.Use();
    glDrawElements(GL_TRIANGLES, gridMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
