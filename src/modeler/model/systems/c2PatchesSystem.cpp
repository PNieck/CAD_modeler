#include <CAD_modeler/model/systems/c2PatchesSystem.hpp>

#include "CAD_modeler/model/components/c2Patches.hpp"
#include "CAD_modeler/model/components/wraps.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/pointsSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/c2PatchesSystem/singleC2Patch.hpp"

#include <numbers>


class C2CylinderSystem;

void C2PatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C2PatchesSystem>();

    coordinator.RegisterRequiredComponent<C2PatchesSystem, C2Patches>();
    coordinator.RegisterRequiredComponent<C2PatchesSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C2PatchesSystem, Mesh>();
}


void C2PatchesSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C2PatchesSystem::CreatePlane(
    const Position &pos, const alg::Vec3 &direction, const float length, const float width
) {
    C2Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2PatchesSystem>());
        }
    }

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, Mesh());
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, PatchesDensity(5));

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);

    return surface;
}


Entity C2PatchesSystem::CreateCylinder(const Position &pos, const alg::Vec3 &direction, float radius)
{
    C2Patches patches(1, 3);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol() - CylinderDoublePointsCnt; ++col) {
            Entity cp = pointsSystem->CreatePoint();
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2PatchesSystem>());
        }
    }

    UpdateDoubleControlPoints(patches);

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, Mesh());
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, PatchesDensity(5));
    coordinator->AddComponent<WrapV>(surface, WrapV());

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculateCylinder(surface, pos, direction, radius);

    return surface;
}


Entity C2PatchesSystem::CreateSurface(C2Patches &patches)
{
    const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < patches.PointsInCol(); ++col) {
            // Creating control points with temporary location
            Entity cp = patches.GetPoint(row, col);

            if (patches.controlPointsHandlers.contains(cp))
                continue;

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistrySys->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C2PatchesSystem>());
        }
    }

    patches.deletionHandler = coordinator->Subscribe<C2Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, Mesh());
    coordinator->AddComponent<C2Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, PatchesDensity(5));

    if (ShouldWrapU(patches))
        coordinator->AddComponent<WrapU>(surface, WrapU());
    if (ShouldWrapV(patches))
        coordinator->AddComponent<WrapV>(surface, WrapV());

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    return surface;
}


void C2PatchesSystem::AddRowOfPlanePatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddRowOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity newEntity = pointSys->CreatePoint(Position());

                patches.SetPoint(newEntity, patches.PointsInRow() - 1, col);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2PatchesSystem>());
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C2PatchesSystem::AddColOfPlanePatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddColOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity newEntity = pointSys->CreatePoint(Position());

                patches.SetPoint(newEntity, row, patches.PointsInCol()-1);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2PatchesSystem>());
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C2PatchesSystem::AddRowOfCylinderPatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float radius
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddRowOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int col=0; col < patches.PointsInCol() - CylinderDoublePointsCnt; col++) {
                Entity newEntity = pointSys->CreatePoint();

                patches.SetPoint(newEntity, patches.PointsInRow()-1, col);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2PatchesSystem>());
            }

            for (int col = 0; col < CylinderDoublePointsCnt; ++col) {
                const Entity cp = patches.GetPoint(patches.PointsInRow()-1, col);
                patches.SetPoint(cp, patches.PointsInRow()-1, patches.PointsInCol() - CylinderDoublePointsCnt + col);
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculateCylinder(surface, pos, direction, radius);
}


void C2PatchesSystem::AddColOfCylinderPatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float radius
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddColOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity newEntity = pointSys->CreatePoint();

                patches.SetPoint(newEntity, row, patches.PointsInCol()-CylinderDoublePointsCnt-1);

                HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                patches.controlPointsHandlers.insert({ newEntity, newHandler });

                cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C2PatchesSystem>());
            }

            UpdateDoubleControlPoints(patches);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculateCylinder(surface, pos, direction, radius);
}


void C2PatchesSystem::DeleteRowOfPlanePatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol(); col++) {
                Entity point = patches.GetPoint(patches.PointsInRow() - 1, col);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2PatchesSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C2PatchesSystem::DeleteColOfPlanePatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol() - 1);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2PatchesSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C2PatchesSystem::DeleteRowOfCylinderPatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const  float radius
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int col=0; col < patches.PointsInCol() - CylinderDoublePointsCnt; col++) {
                Entity point = patches.GetPoint(patches.PointsInRow()-1, col);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2PatchesSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);
    RecalculateCylinder(surface, pos, direction, radius);
}


void C2PatchesSystem::DeleteColOfCylinderPatches(
    Entity surface, const Position &pos, const alg::Vec3 &direction, const float radius
) const {
    coordinator->EditComponent<C2Patches>(surface,
        [surface, this](C2Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (int row=0; row < patches.PointsInRow(); row++) {
                Entity point = patches.GetPoint(row, patches.PointsInCol() - CylinderDoublePointsCnt - 1);
                cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C2PatchesSystem>());
                coordinator->DestroyEntity(point);

                patches.controlPointsHandlers.erase(point);
            }

            patches.DeleteColOfPatches();
            UpdateDoubleControlPoints(patches);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);
    RecalculateCylinder(surface, pos, direction, radius);
}


void C2PatchesSystem::MergeControlPoints(const Entity surface, Entity oldCP, Entity newCP)
{
    if (oldCP == newCP)
        return;

    coordinator->EditComponent<C2Patches>(surface,
        [oldCP, newCP, this] (C2Patches& patches) {
            for (int row=0; row < patches.PointsInRow(); row++) {
                for (int col=0; col < patches.PointsInCol(); col++) {
                    if (patches.GetPoint(row, col) == oldCP)
                        patches.SetPoint(newCP, row, col);
                }
            }

            const HandlerId handlerId = patches.controlPointsHandlers.at(oldCP);

            if (!patches.controlPointsHandlers.contains(newCP)) {
                const auto eventHandler = coordinator->GetEventHandler<Position>(oldCP, handlerId);
                auto newEventHandlerId = coordinator->Subscribe<Position>(newCP, eventHandler);
                patches.controlPointsHandlers.insert({newCP, newEventHandlerId});
            }

            coordinator->Unsubscribe<Position>(oldCP, handlerId);
            patches.controlPointsHandlers.erase(oldCP);
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(surface);

    const auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(surface, oldCP, Coordinator::GetSystemID<C2PatchesSystem>());
    registry->RegisterControlPoint(surface, newCP, Coordinator::GetSystemID<C2PatchesSystem>());

    // Update wrapping
    const auto& c2Patches = coordinator->GetComponent<C2Patches>(surface);

    bool hasWrapping = coordinator->HasComponent<WrapU>(surface);
    bool shouldWrap = ShouldWrapU(c2Patches);

    if (hasWrapping != shouldWrap) {
        if (hasWrapping)
            coordinator->DeleteComponent<WrapU>(surface);
        else
            coordinator->AddComponent<WrapU>(surface, WrapU());
    }

    hasWrapping = coordinator->HasComponent<WrapV>(surface);
    shouldWrap = ShouldWrapV(c2Patches);

    if (hasWrapping != shouldWrap) {
        if (hasWrapping)
            coordinator->DeleteComponent<WrapV>(surface);
        else
            coordinator->AddComponent<WrapV>(surface, WrapV());
    }
}


void C2PatchesSystem::ShowDeBoorNet(const Entity surface) const
{
    auto const& patches = coordinator->GetComponent<C2Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


void C2PatchesSystem::RecalculatePlane(
    const Entity surface, const Position &pos, const alg::Vec3 &direction, const float length, const float width
) const {
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


void C2PatchesSystem::RecalculateCylinder(
    const Entity cylinder, const Position &pos, const alg::Vec3 &direction, const float radius
) const {
    auto const& patches = coordinator->GetComponent<C2Patches>(cylinder);

    const alg::Vec3 offset = direction / static_cast<float>(patches.PointsInRow());

    const Rotation rot(direction, 2.f * std::numbers::pi_v<float> / static_cast<float>(patches.PointsInCol() - CylinderDoublePointsCnt));

    alg::Vec3 v = GetPerpendicularVec(direction) * radius + pos.vec;

    for (int i=0; i < patches.PointsInRow(); ++i) {
        for (int j=0; j < patches.PointsInCol() - CylinderDoublePointsCnt; ++j) {
            const Entity cp = patches.GetPoint(i, j);
            coordinator->SetComponent(cp, Position(v));

            rot.Rotate(v);
        }

        v += offset;
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


alg::Vec4 CubicBSplinesBaseFunctions(const float t)
{
    alg::Vec3 quadratic = QuadraticBSplinesBaseFunctions(t);

    return {
        b(3, 1, t) * quadratic.Z(),
        b(3, 0, t) * quadratic.Y() + a(3, 1, t) * quadratic.Z(),
        b(3, -1, t) * quadratic.X() + a(3, 0, t) * quadratic.Y(),
        a(3, -1, t) * quadratic.X()
    };
}


Position C2PatchesSystem::PointOnSurface(const C2Patches &patches, float u, float v) const
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


alg::Vec3 C2PatchesSystem::PartialDerivativeU(const C2Patches &patches, float u, float v) const
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


alg::Vec3 C2PatchesSystem::PartialDerivativeV(const C2Patches &patches, float u, float v) const
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


void C2PatchesSystem::Update() const
{
    auto const toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    for (const auto entity: toUpdateSystem->GetEntitiesToUpdate<C2PatchesSystem>()) {
        auto const& patches = coordinator->GetComponent<C2Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);
    }

    toUpdateSystem->UnmarkAll<C2PatchesSystem>();
}


void C2PatchesSystem::UpdateMesh(const Entity surface, const C2Patches& patches) const
{
    coordinator->EditComponent<Mesh>(surface,
        [&patches, this](Mesh& mesh) {
            mesh.Update(
                GenerateVertices(patches),
                GenerateIndices(patches)
            );
        }
    );
}


void C2PatchesSystem::UpdateDoubleControlPoints(C2Patches &patches) const
{
    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < CylinderDoublePointsCnt; ++col) {
            const Entity cp = patches.GetPoint(row, col);
            const int column = patches.PointsInCol() - CylinderDoublePointsCnt + col;
            patches.SetPoint(cp, row, column);
        }
    }
}


std::vector<float> C2PatchesSystem::GenerateVertices(const C2Patches &patches) const
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


std::vector<uint32_t> C2PatchesSystem::GenerateIndices(const C2Patches &patches) const
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


bool C2PatchesSystem::ShouldWrapU(const C2Patches &patches)
{
    for (int col=0; col < patches.PointsInCol(); ++col) {
        for (int row=0; row < CylinderDoublePointsCnt; ++row) {
            if (patches.GetPoint(row, col) != patches.GetPoint(patches.PointsInRow() + row - CylinderDoublePointsCnt, col))
                return false;
        }
    }

    return true;
}


bool C2PatchesSystem::ShouldWrapV(const C2Patches &patches)
{
    for (int row=0; row < patches.PointsInRow(); ++row) {
        for (int col=0; col < CylinderDoublePointsCnt; ++col) {
            if (patches.GetPoint(row, col) != patches.GetPoint(row, patches.PointsInCol() + col - CylinderDoublePointsCnt))
                return false;
        }
    }

    return true;
}


void C2PatchesSystem::NormalizeUV(const C2Patches& patches, float& u, float& v)
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


void C2PatchesSystem::DeletionHandler::HandleEvent(const Entity entity, const C2Patches &component, const EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    const auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (const auto [cp, handlerId]: component.controlPointsHandlers) {
        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C2PatchesSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C2PatchesSystem::ControlPointMovedHandler::HandleEvent(
    const Entity entity, const Position& component, const EventType eventType
) {
    (void)entity;
    (void)component;
    (void)eventType;

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C2PatchesSystem>(targetObject);
}
