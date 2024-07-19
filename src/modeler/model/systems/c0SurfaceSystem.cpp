#include <CAD_modeler/model/systems/c0SurfaceSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/curveControlPointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"

#include "CAD_modeler/model/components/c0SurfacePatches.hpp"
#include "CAD_modeler/model/components/c0SurfaceDensity.hpp"
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

    coordinator.RegisterRequiredComponent<C0SurfaceSystem, C0SurfacePatches>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, C0SurfaceDensity>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, Mesh>();
}


void C0SurfaceSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C0SurfaceSystem::CreateSurface(const Position& pos, const alg::Vec3& direction, float length, float width)
{
    static constexpr int controlPointsInOneDir = 4;
    static constexpr int controlPointsCnt = controlPointsInOneDir*controlPointsInOneDir;

    C0SurfacePatches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();

    Entity surface = coordinator->CreateEntity();

    auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int i=0; i < controlPointsInOneDir; ++i) {
        for (int j=0; j < controlPointsInOneDir; ++j) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            patches.SetPoint(cp, 0, 0, i, j);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());
        }
    }

    Mesh mesh;
    C0SurfaceDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C0SurfacePatches>(surface, deletionHandler);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("SurfaceC0_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C0SurfacePatches>(surface, patches);
    coordinator->AddComponent<C0SurfaceDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);
    coordinator->GetSystem<C0PatchesSystem>()->AddPossibilityToHasPatchesPolygon(surface);

    Recalculate(surface, pos, direction, length, width);

    return surface;
}


void C0SurfaceSystem::AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddRow();

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
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddCol();

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
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::DeleteRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this](C0SurfacePatches& patches) {
            for (int col=0; col < patches.PointsInCol(); col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity point = patches.GetPoint(row, col);
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteRow();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::DeleteColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this](C0SurfacePatches& patches) {
            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity point = patches.GetPoint(row, col);
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteCol();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::Recalculate(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    auto const& patches = coordinator->GetComponent<C0SurfacePatches>(surface);

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


void C0SurfaceSystem::DeletionHandler::HandleEvent(Entity entity, const C0SurfacePatches& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    coordinator.EditComponent<C0SurfacePatches>(entity,
        [&component, this](C0SurfacePatches& patches) {
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


void C0SurfaceSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}
