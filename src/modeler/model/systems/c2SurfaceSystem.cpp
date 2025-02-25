#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"

#include <CAD_modeler/utilities/setIntersection.hpp>


void C2SurfaceSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2SurfaceSystem>();

    coordinator.RegisterRequiredComponent<C2SurfaceSystem, C2Patches>();
    coordinator.RegisterRequiredComponent<C2SurfaceSystem, PatchesDensity>();
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
    auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

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

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2SurfaceSystem>());
        }
    }

    Mesh mesh;
    PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);

    return surface;
}


Entity C2SurfaceSystem::CreateSurface(C2Patches &patches)
{
    auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            // Creating control points with temporary location
            Entity cp = patches.GetPoint(row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2SurfaceSystem>());
        }
    }

    Mesh mesh;
    PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    return surface;
}


void C2SurfaceSystem::AddRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
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

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2SurfaceSystem>());
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
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
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

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2SurfaceSystem>());
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
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity point = patches.GetPoint(patches.PointsInRow() - 1, col);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2SurfaceSystem>());
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
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol());
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2SurfaceSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::MergeControlPoints(Entity surface, Entity oldCP, Entity newCP)
{
    coordinator->EditComponent<C2Patches>(surface,
        [oldCP, newCP, this] (C2Patches& patches) {
            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=0; col < patches.PointsInCol(); col++) {
                    if (patches.GetPoint(row, col) == oldCP)
                        patches.SetPoint(newCP, row, col);
                }
            }

            HandlerId handlerId = patches.controlPointsHandlers.at(oldCP);

            if (!patches.controlPointsHandlers.contains(newCP)) {
                auto eventHandler = coordinator->GetEventHandler<Position>(oldCP, handlerId);
                auto newEventHandlerId = coordinator->Subscribe<Position>(newCP, eventHandler);
                patches.controlPointsHandlers.insert({newCP, newEventHandlerId});
            }

            coordinator->Unsubscribe<Position>(oldCP, handlerId);
            patches.controlPointsHandlers.erase(oldCP);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(surface, oldCP, Coordinator::GetSystemID<C2SurfaceSystem>());
    registry->RegisterControlPoint(surface, newCP, Coordinator::GetSystemID<C2SurfaceSystem>());
}


void C2SurfaceSystem::ShowDeBoorNet(Entity surface)
{
    auto const& patches = coordinator->GetComponent<C2Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
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


void C2SurfaceSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBSplineSurfaceShader();

    UpdateEntities();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        auto const& density = coordinator->GetComponent<PatchesDensity>(entity);
        float v[] = {5.f, 64.f, 64.f, 64.f};
        v[0] = static_cast<float>(density.GetDensity());
        glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, v);

	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void C2SurfaceSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        auto const& patches = coordinator->GetComponent<C2Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);

        toUpdateSystem->Unmark(entity);
    }
}


void C2SurfaceSystem::UpdateMesh(Entity surface, const C2Patches& patches) const
{
    coordinator->EditComponent<Mesh>(surface,
        [surface, &patches, this](Mesh& mesh) {
            mesh.Update(
                GenerateVertices(patches),
                GenerateIndices(patches)
            );
        }
    );
}


std::vector<float> C2SurfaceSystem::GenerateVertices(const C2Patches &patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * 3);

    for (int col=0; col < patches.PointsInCol(); col++) {
        for (int row=0; row < patches.PointsInRow(); row++) {
            Entity point = patches.GetPoint(row, col);

            auto const& pos = coordinator->GetComponent<Position>(point);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C2SurfaceSystem::GenerateIndices(const C2Patches &patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PatchesInRow() * patches.PatchesInCol() * C2Patches::PointsInPatch * 2);

    for (int patchRow=0; patchRow < patches.PatchesInRow(); patchRow++) {
        for (int patchCol=0; patchCol < patches.PatchesInCol(); patchCol++) {
            for (int rowInPatch=0; rowInPatch < C2Patches::RowsInPatch; rowInPatch++) {
                for (int colInPatch=0; colInPatch < C2Patches::ColsInPatch; colInPatch++) {

                    int globCol = patchCol + colInPatch;
                    int globRow = patchRow + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C2Patches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C2Patches::RowsInPatch; rowInPatch++) {

                    int globCol = patchCol + colInPatch;
                    int globRow = patchRow + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}


// TODO: merge with other deletion handlers
void C2SurfaceSystem::DeletionHandler::HandleEvent(Entity entity, const C2Patches &component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (auto handler: component.controlPointsHandlers) {
        Entity cp = handler.first;
        HandlerId handlerId = handler.second;

        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C2SurfaceSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C2SurfaceSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}
