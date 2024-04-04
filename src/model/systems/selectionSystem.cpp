#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/model/ecsCoordinator.hpp>

#include <CAD_modeler/model/components/selected.hpp>

#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <stdexcept>


void SelectionSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<SelectionSystem>();

    coordinator.RegisterComponent<Selected>();

    coordinator.RegisterRequiredComponent<SelectionSystem, Selected>();
}


void SelectionSystem::Select(Entity entity)
{
    coordinator->AddComponent<Selected>(entity, Selected());
}


void SelectionSystem::Deselect(Entity entity)
{
    coordinator->DeleteComponent<Selected>(entity);
}


void SelectionSystem::SelectFromLine(const Line& line)
{
    auto const& pointsSystem = coordinator->GetSystem<PointsSystem>();

    float smallestT = std::numeric_limits<float>::infinity();
    Entity actEntity;

    for (auto const entity: pointsSystem->GetEntities()) {
        auto const& position = coordinator->GetComponent<Position>(entity);

        float t = glm::dot(line.GetDirection(), position.vec - line.GetSamplePoint());
        glm::vec3 projection = line.GetPointOnLine(t);

        // TODO: change to distance Squared
        float distance = glm::distance(projection, position.vec);

        if (t < smallestT && distance < 0.5) {
            smallestT = t;
            actEntity = entity;
        }
    }

    if (smallestT != std::numeric_limits<float>::infinity()) {
        if (IsSelected(actEntity))
            Deselect(actEntity);
        else
            Select(actEntity);
    }
}
