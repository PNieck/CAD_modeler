#include <CAD_modeler/model/systems/c0SurfaceSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"
#include "CAD_modeler/model/systems/controlNetSystem.hpp"

#include "CAD_modeler/model/components/c0Patches.hpp"
#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/name.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <vector>
#include <stack>


void C0SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0SurfaceSystem>();

    coordinator.RegisterRequiredComponent<C0SurfaceSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, Mesh>();
}


void C0SurfaceSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C0SurfaceSystem::CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width)
{
    C0Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    auto cpRegistrySystem = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int i=0; i < patches.PointsInCol(); ++i) {
        for (int j=0; j < patches.PointsInRow(); ++j) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            patches.SetPoint(cp, i, j);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());

            cpRegistrySystem->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C0SurfaceSystem>());
        }
    }

    Mesh mesh;
    PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C0Patches>(surface, deletionHandler);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("SurfaceC0_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C0Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);

    return surface;
}


void C0SurfaceSystem::AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddRowOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol(); col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                    cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C0SurfaceSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddColOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                    cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C0SurfaceSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol(); col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C0SurfaceSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C0SurfaceSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::ShowBezierNet(Entity surface) const
{
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


void C0SurfaceSystem::Recalculate(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);

    alg::Vec3 perpendicular1 = alg::GetPerpendicularVec(direction);
    alg::Vec3 perpendicular2 = alg::Cross(perpendicular1, direction);

    perpendicular1 *= length / float(patches.PointsInRow() - 1);
    perpendicular2 *= width / float(patches.PointsInCol() - 1);

    for (int i=0; i < patches.PointsInRow(); ++i) {
        for (int j=0; j < patches.PointsInCol(); ++j) {
            Entity cp = patches.GetPoint(i, j);

            alg::Vec3 newPos = pos.vec + perpendicular1 * float(i) + perpendicular2 * float(j);

            coordinator->SetComponent(cp, Position(newPos));
        }
    }
}


// TODO: merge with other deletion handlers
void C0SurfaceSystem::DeletionHandler::HandleEvent(Entity entity, const C0Patches& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (auto handler: component.controlPointsHandlers) {
        Entity cp = handler.first;
        HandlerId handlerId = handler.second;

        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C0SurfaceSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C0SurfaceSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}
