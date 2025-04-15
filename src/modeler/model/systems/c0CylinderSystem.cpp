#include <CAD_modeler/model/systems/c0CylinderSystem.hpp>

#include <ecs/coordinator.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"

#include "CAD_modeler/model/components/c0Patches.hpp"
#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/rotation.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"
#include "CAD_modeler/model/components/IsC0Cylinder.hpp"

#include <numbers>


void C0CylinderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0CylinderSystem>();

    coordinator.RegisterRequiredComponent<C0CylinderSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, Mesh>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, IsC0Cylinder>();
}


void C0CylinderSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C0CylinderSystem::CreateCylinder(const Position &pos, const alg::Vec3 &direction, float radius)
{
    C0Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    const auto cpRegistry = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity cylinder = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(cylinder, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol() - 1; ++col) {
            Entity cp = pointsSystem->CreatePoint();
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());

            cpRegistry->RegisterControlPoint(cylinder, cp, Coordinator::GetSystemID<C0CylinderSystem>());
        }
    }

    for (int row=0; row < patches.PointsInRow(); ++row) {
        const Entity cp = patches.GetPoint(row, 0);
        patches.SetPoint(cp, row, patches.PointsInCol() - 1);
    }

    const Mesh mesh;
    const PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C0Patches>(cylinder, deletionHandler);

    coordinator->AddComponent<Mesh>(cylinder, mesh);
    coordinator->AddComponent<C0Patches>(cylinder, patches);
    coordinator->AddComponent<PatchesDensity>(cylinder, density);
    coordinator->AddComponent<IsC0Cylinder>(cylinder, {});

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    Recalculate(cylinder, pos, direction, radius);

    return cylinder;
}


void C0CylinderSystem::AddRowOfPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddRowOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol() - 1; col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint();

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                    cpRegistrySys->RegisterControlPoint(cylinder, newEntity, Coordinator::GetSystemID<C0CylinderSystem>());
                }
            }

            for (int row = patches.PointsInRow() - 3; row < patches.PointsInRow(); ++row) {
                Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    Recalculate(cylinder, pos, direction, radius);
}


void C0CylinderSystem::AddColOfPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddColOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); ++row) {
                Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
                    Entity newEntity = pointSys->CreatePoint();

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                    cpRegistrySys->RegisterControlPoint(cylinder, newEntity, Coordinator::GetSystemID<C0CylinderSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    Recalculate(cylinder, pos, direction, radius);
}


void C0CylinderSystem::DeleteRowOfPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol() - 1; col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(cylinder, point, Coordinator::GetSystemID<C0CylinderSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);
    Recalculate(cylinder, pos, direction, radius);
}


void C0CylinderSystem::DeleteColOfPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(cylinder, point, Coordinator::GetSystemID<C0CylinderSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol() - 1);
                patches.SetPoint(point, row, patches.PointsInCol() - 4);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);
    Recalculate(cylinder, pos, direction, radius);
}


void C0CylinderSystem::ShowBezierPolygon(Entity cylinder) const
{
    auto const& patches = coordinator->GetComponent<C0Patches>(cylinder);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(cylinder, patches);
}


void C0CylinderSystem::Recalculate(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    auto const& patches = coordinator->GetComponent<C0Patches>(cylinder);

    alg::Vec3 offset = direction / float(patches.PointsInRow());

    Rotation rot(direction, 2.f * std::numbers::pi_v<float> / float(patches.PointsInCol() - 1));

    alg::Vec3 v = alg::GetPerpendicularVec(direction) * radius + pos.vec;

    for (int i=0; i < patches.PointsInRow(); ++i) {
        for (int j=0; j < patches.PointsInCol() - 1; ++j) {
            Entity cp = patches.GetPoint(i, j);
            coordinator->SetComponent(cp, Position(v));

            rot.Rotate(v);
        }

        v += offset;
    }
}


// TODO: merge with other deletion handlers
void C0CylinderSystem::DeletionHandler::HandleEvent(Entity entity, const C0Patches& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (auto handler: component.controlPointsHandlers) {
        Entity cp = handler.first;
        HandlerId handlerId = handler.second;

        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C0CylinderSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C0CylinderSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    (void)entity;
    (void)component;
    (void)eventType;

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(targetObject);
}
