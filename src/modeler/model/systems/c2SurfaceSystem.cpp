#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/unremovable.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/shaders/shaderRepository.hpp"
#include "CAD_modeler/model/systems/c2SurfacesSystem/singleC2Patch.hpp"


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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

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

            if (patches.controlPointsHandlers.contains(cp))
                continue;

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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::DeleteRowOfPatches(Entity surface, const Position &pos, const alg::Vec3 &direction, float length, float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity point = patches.GetPoint(patches.PointsInRow() - 1, col);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2SurfaceSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

    Recalculate(surface, pos, direction, length, width);
}


void C2SurfaceSystem::DeleteColOfPatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width) const
{
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol() - 1);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2SurfaceSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(surface);

    const auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(surface, oldCP, Coordinator::GetSystemID<C2SurfaceSystem>());
    registry->RegisterControlPoint(surface, newCP, Coordinator::GetSystemID<C2SurfaceSystem>());
}


void C2SurfaceSystem::ShowDeBoorNet(const Entity surface)
{
    auto const& patches = coordinator->GetComponent<C2Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


void C2SurfaceSystem::Recalculate(
    const Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width) const
{
    auto const& patches = coordinator->GetComponent<C2Patches>(surface);

    alg::Vec3 perpendicular1 = GetPerpendicularVec(direction);
    alg::Vec3 perpendicular2 = Cross(perpendicular1, direction);

    perpendicular1 *= length / static_cast<float>(patches.PointsInRow() - 1);
    perpendicular2 *= width / static_cast<float>(patches.PointsInCol() - 1);

    for (int i=0; i < patches.PointsInRow(); ++i) {
        for (int j=0; j < patches.PointsInCol(); ++j) {
            const Entity cp = patches.GetPoint(i, j);

            alg::Vec3 newPos = pos.vec + perpendicular1 * static_cast<float>(i) + perpendicular2 * static_cast<float>(j);

            coordinator->SetComponent(cp, Position(newPos));
        }
    }
}


// CubicBSplinesBaseFunctions helper function
float a(const int n, const int j, const float t) {
    return (static_cast<float>(j + n - 1) - t)/static_cast<float>(n);
}


// CubicBSplinesBaseFunctions helper function
float b(const int n, const int j, const float t) {
    return (t - static_cast<float>(j - 1))/static_cast<float>(n);
}


alg::Vec3 QuadraticBSplinesBaseFunctions(const float t)
{
    alg::Vec3 result;
    result.X() = 1;

    result.Y() = b(1, 1, t) * result.X();
    result.X() = a(1, 1, t) * result.X();

    result.Z() = b(2, 1, t) * result.Y();
    result.Y() = b(2, 0, t) * result.X() + a(2, 1, t) * result.Y();
    result.X() = a(2, 0, t) * result.X();

    return result;
}


alg::Vec4 CubicBSplinesBaseFunctions(const float t) {
    alg::Vec3 quadratic = QuadraticBSplinesBaseFunctions(t);

    return {
        b(3, 1, t) * quadratic.Z(),
        b(3, 0, t) * quadratic.Y() + a(3, 1, t) * quadratic.Z(),
        b(3, -1, t) * quadratic.X() + a(3, 0, t) * quadratic.Y(),
        a(3, -1, t) * quadratic.X()
    };
}


Position C2SurfaceSystem::PointOnPatches(const C2Patches &patches, float u, float v) const
{
    const SingleC2Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    alg::Vec4 Nu = CubicBSplinesBaseFunctions(u);
    alg::Vec4 Nv = CubicBSplinesBaseFunctions(v);

    Position result(0.f);

    for (int i=0; i <= 3; i++) {
        for (int j=0; j <= 3; j++) {
            result.vec += p.Point(3 - i, 3 - j) * Nu[i] * Nv[j];
        }
    }

    return result;
}


alg::Vec3 C2SurfaceSystem::PartialDerivativeU(const C2Patches &patches, float u, float v) const
{
    const SingleC2Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    alg::Vec3 Nu = QuadraticBSplinesBaseFunctions(u);
    alg::Vec4 Nv = CubicBSplinesBaseFunctions(v);

    alg::Vec3 result(0.f);

    for (int i=0; i <= 2; i++) {
        for (int j=0; j <= 3; j++) {
            result += (p.Point(i+1, 3-j) - p.Point(i, 3-j)) * Nu[i] * Nv[j];
        }
    }

    return result;
}


alg::Vec3 C2SurfaceSystem::PartialDerivativeV(const C2Patches &patches, float u, float v) const
{
    const SingleC2Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    alg::Vec4 Nu = CubicBSplinesBaseFunctions(u);
    alg::Vec3 Nv = QuadraticBSplinesBaseFunctions(v);

    alg::Vec3 result(0.f);

    for (int i=0; i <= 3; i++) {
        for (int j=0; j <= 2; j++) {
            result += (p.Point(3-i, j+1) - p.Point(3-i, j)) * Nu[i] * Nv[j];
        }
    }

    return result;
}


void C2SurfaceSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBSplineSurfaceShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    glPatchParameteri(GL_PATCH_VERTICES, 16);

    for (auto const entity: entities) {
        const bool selection = selectionSystem->IsSelected(entity);

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


void C2SurfaceSystem::Update() const
{
    auto const toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    for (const auto entity: toUpdateSystem->GetEntitiesToUpdate<C2SurfaceSystem>()) {
        auto const& patches = coordinator->GetComponent<C2Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);
    }

    toUpdateSystem->UnmarkAll<C2SurfaceSystem>();
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
            const Entity point = patches.GetPoint(row, col);

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

                    const int globCol = patchCol + colInPatch;
                    const int globRow = patchRow + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C2Patches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C2Patches::RowsInPatch; rowInPatch++) {

                    const int globCol = patchCol + colInPatch;
                    const int globRow = patchRow + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}


void C2SurfaceSystem::NormalizeUV(const C2Patches& patches, float& u, float& v)
{
    if (u == MaxU(patches))
        u = 1.f;
    else
        u -= std::floor(u);

    if (v == MaxV(patches))
        v = 1.f;
    else
        v -= std::floor(v);
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
    (void)entity;
    (void)component;
    (void)eventType;

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2SurfaceSystem>(targetObject);
}
