#include <CAD_modeler/model/systems/gridSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>


void GridSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<GridSystem>();

    coordinator.RegisterComponent<Mesh>();
}


void GridSystem::Init()
{
    grid = coordinator->CreateEntity();

    Mesh mesh;
    const std::vector vertices = {
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    const std::vector<uint32_t> indices = {
        0, 1, 3, // First triangle
        1, 2, 3  // Second triangle
    };

    mesh.Update(vertices, indices);

    coordinator->AddComponent<Mesh>(grid, mesh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void GridSystem::Render(const alg::Mat4x4& viewMtx, const alg::Mat4x4& projMtx, const float nearPlane, const float farPlane) const
{
    auto const& gridMesh = coordinator->GetComponent<Mesh>(grid);
    auto const& gridShader = ShaderRepository::GetInstance().GetGridShader();

    gridShader.Use();
    gridShader.SetFarPlane(farPlane);
    gridShader.SetNearPlane(nearPlane);
    gridShader.SetViewMatrix(viewMtx);
    gridShader.SetProjectionMatrix(projMtx);
    
    gridMesh.Use();
    glDrawElements(GL_TRIANGLES, gridMesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
}
