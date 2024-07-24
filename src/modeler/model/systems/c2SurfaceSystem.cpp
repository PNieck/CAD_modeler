#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"
#include "CAD_modeler/model/components/name.hpp"

#include "CAD_modeler/model/systems/cameraSystem.hpp"
#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/curveControlPointsSystem.hpp"


void C2SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2SurfaceSystem>();

    coordinator.RegisterRequiredComponent<C2SurfaceSystem, C2Patches>();
    coordinator.RegisterRequiredComponent<C2SurfaceSystem, C0PatchesDensity>();
    coordinator.RegisterRequiredComponent<C2SurfaceSystem, Mesh>();
}


void C2SurfaceSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C2SurfaceSystem::CreateSurface(const Position &pos, const alg::Vec3 &direction, float length, float width)
{
    C2Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();

    Entity surface = coordinator->CreateEntity();

    auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());
        }
    }

    Mesh mesh;
    C0PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("SurfaceC2_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<C0PatchesDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);
    //coordinator->GetSystem<C0PatchesSystem>()->AddPossibilityToHasPatchesPolygon(surface);

    Recalculate(surface, pos, direction, length, width);

    return surface;
}


void C2SurfaceSystem::AddRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddRowOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity newEntity = pointSys->CreatePoint(Position());

                patches.SetPoint(newEntity, patches.PointsInRow() - 1, col);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                coordinator->AddComponent<Unremovable>(newEntity, Unremovable());
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::AddColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddColOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity newEntity = pointSys->CreatePoint(Position());

                patches.SetPoint(newEntity, row, patches.PointsInCol()-1);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                coordinator->AddComponent<Unremovable>(newEntity, Unremovable());
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::DeleteRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity point = patches.GetPoint(patches.PointsInRow() - 1, col);
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::DeleteColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::Recalculate(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    auto const& patches = coordinator->GetComponent<C2Patches>(surface);

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


void C2SurfaceSystem::DeletionHandler::HandleEvent(Entity entity, const C2Patches &component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    coordinator.EditComponent<C2Patches>(entity,
        [&component, this](C2Patches& patches) {
            auto controlPointsSystem = coordinator.GetSystem<CurveControlPointsSystem>();

            for (int col=0; col < component.PointsInCol(); col++) {
                for (int row=0; row < component.PointsInRow(); row++) {
                    Entity controlPoint = component.GetPoint(row, col);

                    coordinator.Unsubscribe<Position>(controlPoint, patches.controlPointsHandlers.at(controlPoint));

                    if (!controlPointsSystem->IsAControlPoint(controlPoint))
                        coordinator.DestroyEntity(controlPoint);
                }
            }
        }
    );
}


void C2SurfaceSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}
