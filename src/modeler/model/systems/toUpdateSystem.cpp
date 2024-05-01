#include <CAD_modeler/model/systems/toUpdateSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/toUpdate.hpp>


void ToUpdateSystem::RegisterSystem(Coordinator& coordinator)
{
    coordinator.RegisterSystem<ToUpdateSystem>();

    coordinator.RegisterRequiredComponent<ToUpdateSystem, ToUpdate>();
}
