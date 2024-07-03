#include <CAD_modeler/model/systems/c0CylinderSystem.hpp>

#include <ecs/coordinator.hpp>

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem.hpp"

#include "CAD_modeler/model/components/c0SurfacePatches.hpp"
#include "CAD_modeler/model/components/c0SurfaceDensity.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/name.hpp"
#include "CAD_modeler/model/components/controlPoints.hpp"
#include "CAD_modeler/model/components/rotation.hpp"

#include <numbers>


void C0CylinderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0CylinderSystem>();

    coordinator.RegisterRequiredComponent<C0CylinderSystem, C0SurfacePatches>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, ControlPoints>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, C0SurfaceDensity>();
    coordinator.RegisterRequiredComponent<C0CylinderSystem, Mesh>();
}


Entity C0CylinderSystem::CreateCylinder(const Position &pos, const alg::Vec3 &direction, float radius)
{
    static constexpr int controlPointsInOneDir = 4;
    static constexpr int controlPointsCnt = controlPointsInOneDir*controlPointsInOneDir;

    std::vector<Entity> controlPoints;
    C0SurfacePatches patches(1, 1);
    controlPoints.reserve(controlPointsCnt);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();

    for (int i=0; i < controlPointsInOneDir; ++i) {
        for (int j=0; j < controlPointsInOneDir - 1; ++j) {
            Entity cp = pointsSystem->CreatePoint(Position());
            controlPoints.push_back(cp);
            patches.SetPoint(cp, 0, 0, i, j);
        }
    }

    for (int i=0; i < controlPointsInOneDir; ++i) {
        Entity cp = patches.GetPoint(i, 0);
        patches.SetPoint(cp, i, controlPointsInOneDir - 1);
    }

    auto const controlPointsSys = coordinator->GetSystem<ControlPointsSystem>();
    Entity surface = controlPointsSys->CreateControlPoints(controlPoints);

    Mesh mesh;

    C0SurfaceDensity density(5);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("CylinderC0_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C0SurfacePatches>(surface, patches);
    coordinator->AddComponent<C0SurfaceDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    RecalculatePositions(surface, pos, direction, radius);

    coordinator->GetSystem<C0PatchesSystem>()->AddPossibilityToHasPatchesPolygon(surface);

    return surface;
}


void C0CylinderSystem::AddRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    std::stack<Entity> newEntities;

    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this, &newEntities](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddRow();

            for (int col=0; col < patches.PointsInCol() - 1; col++) {
                for (int row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);
                    newEntities.push(newEntity);
                }
            }

            for (int row = patches.PointsInRow() - 3; row < patches.PointsInRow(); ++row) {
                Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }
        }
    );

    auto ctrlPointsSys = coordinator->GetSystem<ControlPointsSystem>();

    while (!newEntities.empty()) {
        ctrlPointsSys->AddControlPoint(surface, newEntities.top());
        newEntities.pop();
    }

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    RecalculatePositions(surface, pos, direction, radius);
}


void C0CylinderSystem::AddColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    std::stack<Entity> newEntities;

    coordinator->EditComponent<C0SurfacePatches>(surface,
        [surface, this, &newEntities](C0SurfacePatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            
            patches.AddCol();

            for (int row=0; row < patches.PointsInRow(); ++row) {
                Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }

            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
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

    RecalculatePositions(surface, pos, direction, radius);
}


void C0CylinderSystem::DeleteRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
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
    RecalculatePositions(surface, pos, direction, radius);
}


void C0CylinderSystem::DeleteColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
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
    RecalculatePositions(surface, pos, direction, radius);
}


void C0CylinderSystem::RecalculatePositions(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    auto const& patches = coordinator->GetComponent<C0SurfacePatches>(cylinder);

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
