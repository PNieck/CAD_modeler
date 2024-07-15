#include <CAD_modeler/model/systems/c0SurfaceSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/vec3.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"

#include "CAD_modeler/model/components/c0SurfacePatches.hpp"
#include "CAD_modeler/model/components/c0SurfaceDensity.hpp"
#include "CAD_modeler/model/components/controlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/name.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <vector>
#include <stack>


void C0SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0SurfaceSystem>();

    coordinator.RegisterRequiredComponent<C0SurfaceSystem, C0SurfacePatches>();
    coordinator.RegisterRequiredComponent<C0SurfaceSystem, ControlPoints>();
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

    std::vector<Entity> controlPoints;
    C0SurfacePatches patches(1, 1);
    controlPoints.reserve(controlPointsCnt);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();

    for (int i=0; i < controlPointsInOneDir; ++i) {
        for (int j=0; j < controlPointsInOneDir; ++j) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            controlPoints.push_back(cp);
            patches.SetPoint(cp, 0, 0, i, j);
        }
    }

    auto const controlPointsSys = coordinator->GetSystem<ControlPointsSystem>();
    Entity surface = controlPointsSys->CreateControlPoints(controlPoints);

    Mesh mesh;

    C0SurfaceDensity density(5);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("SurfaceC0_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C0SurfacePatches>(surface, patches);
    coordinator->AddComponent<C0SurfaceDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);
    coordinator->GetSystem<C0PatchesSystem>()->AddPossibilityToHasPatchesPolygon(surface);

    coordinator->Subscribe<C0SurfacePatches>(surface, deletionHandler);

    Recalculate(surface, pos, direction, length, width);

    return surface;
}


void C0SurfaceSystem::AddRowOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    std::stack<Entity> newEntities;

    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this, &newEntities](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddRow();

            for (int col=0; col < patches.PointsInCol(); col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);
                    newEntities.push(newEntity);
                }
            }
        }
    );

    // TODO: add adding multiple points at once
    auto ctrlPointsSys = coordinator->GetSystem<ControlPointsSystem>();

    while (!newEntities.empty()) {
        ctrlPointsSys->AddControlPoint(surface, newEntities.top());
        newEntities.pop();
    }

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C0SurfaceSystem::AddColOfPatches(Entity surface, const Position& pos, const alg::Vec3& direction, float length, float width) const
{
    std::stack<Entity> newEntities;

    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this, &newEntities](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddCol();

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);
                    newEntities.push(newEntity);
                }
            }
        }
    );

    // TODO: add adding multiple points at once
    auto ctrlPointsSys = coordinator->GetSystem<ControlPointsSystem>();

    while (!newEntities.empty()) {
        ctrlPointsSys->AddControlPoint(surface, newEntities.top());
        newEntities.pop();
    }

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

    auto controlPointsSystem = coordinator.GetSystem<ControlPointsSystem>();

    for (int col=0; col < component.PointsInCol(); col++) {
        for (int row=0; row < component.PointsInRow(); row++) {
            Entity controlPoint = component.GetPoint(row, col);

            controlPointsSystem->DeleteControlPoint(entity, controlPoint);

            if (!controlPointsSystem->IsAControlPoint(controlPoint))
                coordinator.DestroyEntity(controlPoint);
        }
    }
}
