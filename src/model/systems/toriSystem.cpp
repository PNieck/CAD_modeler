#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <CAD_modeler/model/components/scale.hpp>
#include <CAD_modeler/model/components/rotation.hpp>
#include <CAD_modeler/model/components/mesh.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>

#include <vector>


void ToriSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ToriSystem>();

    coordinator.RegisterComponent<Position>();
    coordinator.RegisterComponent<Scale>();
    coordinator.RegisterComponent<Rotation>();
    coordinator.RegisterComponent<Mesh>();
    coordinator.RegisterComponent<TorusParameters>();

    coordinator.RegisterRequiredComponent<ToriSystem, Position>();
    coordinator.RegisterRequiredComponent<ToriSystem, Scale>();
    coordinator.RegisterRequiredComponent<ToriSystem, Rotation>();
    coordinator.RegisterRequiredComponent<ToriSystem, Mesh>();
    coordinator.RegisterRequiredComponent<ToriSystem, TorusParameters>();
}

Entity ToriSystem::AddTorus(const Position& pos, const TorusParameters& params) const
{
    Entity torus = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(torus, pos);

    Scale scale;
    coordinator->AddComponent<Scale>(torus, scale);

    Rotation rot;
    coordinator->AddComponent<Rotation>(torus, rot);

    coordinator->AddComponent<TorusParameters>(torus, params);

    auto vertices = params.GenerateVertices();
    auto indices = params.GenerateEdges();

    Mesh mesh;
    mesh.Update(vertices, indices);
    coordinator->AddComponent<Mesh>(torus, mesh);

    return torus;
}
