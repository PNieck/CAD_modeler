#include <CAD_modeler/model/systems/c2CylinderSystem.hpp>

#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"
#include "CAD_modeler/model/components/name.hpp"
#include "CAD_modeler/model/components/rotation.hpp"

#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/controlNetSystem.hpp"

#include <numbers>

#include <CAD_modeler/utilities/setIntersection.hpp>


void C2CylinderSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2CylinderSystem>();

    coordinator.RegisterRequiredComponent<C2CylinderSystem, C2CylinderPatches>();
    coordinator.RegisterRequiredComponent<C2CylinderSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C2CylinderSystem, Mesh>();
}


void C2CylinderSystem::Init(ShaderRepository* shadersRepo)
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
    this->shaderRepo = shadersRepo;
}


Entity C2CylinderSystem::CreateCylinder(const Position &pos, const alg::Vec3 &direction, float radius)
{
    C2CylinderPatches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol()-doublePointsCnt; ++col) {
            Entity cp = pointsSystem->CreatePoint();
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            coordinator->AddComponent<Unremovable>(cp, Unremovable());

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2CylinderSystem>());
        }
    }

    UpdateDoubleControlPoints(patches);

    Mesh mesh;
    PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C2CylinderPatches>(surface, deletionHandler);

    coordinator->AddComponent<Name>(surface, nameGenerator.GenerateName("CylinderC2_"));
    coordinator->AddComponent<Mesh>(surface, mesh);
    coordinator->AddComponent<C2CylinderPatches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, density);

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, radius);

    return surface;
}


void C2CylinderSystem::AddRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C2CylinderPatches>(surface,
        [surface, this](C2CylinderPatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddRowOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol() - doublePointsCnt; col++) {
                Entity newEntity = pointSys->CreatePoint();

                patches.SetPoint(newEntity, patches.PointsInRow()-1, col);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2CylinderSystem>());
            }

            for (int col = 0; col < doublePointsCnt; ++col) {
                Entity cp = patches.GetPoint(patches.PointsInRow()-1, col);
                patches.SetPoint(cp, patches.PointsInRow()-1, patches.PointsInCol() - doublePointsCnt + col);
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, radius);
}


void C2CylinderSystem::AddColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C2CylinderPatches>(surface,
        [surface, this](C2CylinderPatches& patches) {
            auto pointSys = coordinator->GetSystem<PointsSystem>();
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();
            
            patches.AddColOfPatches();

            Entity firstCP = patches.GetPoint(0,0);
            HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity newEntity = pointSys->CreatePoint();

                patches.SetPoint(newEntity, row, patches.PointsInCol()-doublePointsCnt-1);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                coordinator->AddComponent<Unremovable>(newEntity, Unremovable());

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2CylinderSystem>());
            }

            UpdateDoubleControlPoints(patches);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);

    Recalculate(surface, pos, direction, radius);
}


void C2CylinderSystem::DeleteRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C2CylinderPatches>(surface,
        [surface, this](C2CylinderPatches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol() - doublePointsCnt; col++) {
                Entity point = patches.GetPoint(patches.PointsInRow()-1, col);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2CylinderSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);
    Recalculate(surface, pos, direction, radius);
}


void C2CylinderSystem::DeleteColOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    coordinator->EditComponent<C2CylinderPatches>(surface,
        [surface, this](C2CylinderPatches& patches) {
            auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol()-doublePointsCnt-1);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2CylinderSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
            UpdateDoubleControlPoints(patches);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(surface);
    Recalculate(surface, pos, direction, radius);
}


void C2CylinderSystem::MergeControlPoints(Entity cylinder, Entity oldCP, Entity newCP)
{
    coordinator->EditComponent<C2CylinderPatches>(cylinder,
        [oldCP, newCP, this] (C2CylinderPatches& patches) {
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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate(cylinder);

    auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(cylinder, oldCP, Coordinator::GetSystemID<C2CylinderSystem>());
    registry->RegisterControlPoint(cylinder, newCP, Coordinator::GetSystemID<C2CylinderSystem>());
}


void C2CylinderSystem::ShowDeBoorNet(Entity cylinder)
{
    auto const& patches = coordinator->GetComponent<C2CylinderPatches>(cylinder);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(cylinder, patches);
}


void C2CylinderSystem::Recalculate(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius) const
{
    constexpr int doubleControlPoints = 3;

    auto const& patches = coordinator->GetComponent<C2CylinderPatches>(cylinder);

    alg::Vec3 offset = direction / float(patches.PointsInRow());

    Rotation rot(direction, 2.f * std::numbers::pi_v<float> / float(patches.PointsInCol() - doubleControlPoints));

    alg::Vec3 v = alg::GetPerpendicularVec(direction) * radius + pos.vec;

    for (int i=0; i < patches.PointsInRow(); ++i) {
        for (int j=0; j < patches.PointsInCol() - doubleControlPoints; ++j) {
            Entity cp = patches.GetPoint(i, j);
            coordinator->SetComponent(cp, Position(v));

            rot.Rotate(v);
        }

        v += offset;
    }
}


// TODO: merge with other deletion handlers
void C2CylinderSystem::DeletionHandler::HandleEvent(Entity entity, const C2CylinderPatches& component, EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (auto handler: component.controlPointsHandlers) {
        Entity cp = handler.first;
        HandlerId handlerId = handler.second;

        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C2CylinderSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C2CylinderSystem::ControlPointMovedHandler::HandleEvent(Entity entity, const Position& component, EventType eventType)
{
    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate(targetObject);
}


void C2CylinderSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shaderRepo->GetBSplineSurfaceShader();

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


void C2CylinderSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        auto const& patches = coordinator->GetComponent<C2CylinderPatches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);

        toUpdateSystem->Unmark(entity);
    }
}


void C2CylinderSystem::UpdateDoubleControlPoints(C2CylinderPatches &patches) const
{
    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < doublePointsCnt; ++col) {
            Entity cp = patches.GetPoint(row, col);
            int column = patches.PointsInCol() - doublePointsCnt + col;
            patches.SetPoint(cp, row, column);
        }
    }
}


void C2CylinderSystem::UpdateMesh(Entity surface, const C2CylinderPatches& patches) const
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


std::vector<float> C2CylinderSystem::GenerateVertices(const C2Patches &patches) const
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


std::vector<uint32_t> C2CylinderSystem::GenerateIndices(const C2Patches &patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PatchesInRow() * patches.PatchesInCol() * C2Patches::PointsInPatch * 2);

    for (int patchRow=0; patchRow < patches.PatchesInRow(); patchRow++) {
        for (int patchCol=0; patchCol < patches.PatchesInCol()+2; patchCol++) {
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
