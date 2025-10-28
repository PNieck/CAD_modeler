#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>

#include "CAD_modeler/model/components/patchesDensity.hpp"
#include "CAD_modeler/model/components/curveControlPoints.hpp"
#include "CAD_modeler/model/components/mesh.hpp"
#include "CAD_modeler/model/components/wraps.hpp"

#include "CAD_modeler/model/systems/selectionSystem.hpp"
#include "CAD_modeler/model/systems/toUpdateSystem.hpp"
#include "CAD_modeler/model/systems/controlPointsRegistrySystem.hpp"
#include "CAD_modeler/model/systems/c0PatchesSystem/singleC0Patch.hpp"
#include "CAD_modeler/model/systems/pointsSystem.hpp"

#include <numbers>


void C0PatchesSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<C0PatchesSystem>();
    coordinator.RegisterSystem<ToUpdateSystem>();
    coordinator.RegisterSystem<ControlNetSystem>();
    coordinator.RegisterSystem<SelectionSystem>();
    coordinator.RegisterSystem<ControlPointsRegistrySystem>();

    coordinator.RegisterComponent<WrapU>();
    coordinator.RegisterComponent<WrapV>();

    coordinator.RegisterRequiredComponent<C0PatchesSystem, C0Patches>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, PatchesDensity>();
    coordinator.RegisterRequiredComponent<C0PatchesSystem, Mesh>();
}


void C0PatchesSystem::Init()
{
    deletionHandler = std::make_shared<DeletionHandler>(*coordinator);
}


Entity C0PatchesSystem::CreateCylinder(const Position &pos, const alg::Vec3 &direction, const float radius)
{
    C0Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    const auto cpRegistry = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity cylinder = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(cylinder, *coordinator);

    for (size_t row=0; row < patches.PointsInRow(); ++row) {
        for (size_t col=0; col < patches.PointsInCol() - 1; ++col) {
            Entity cp = pointsSystem->CreatePoint();
            patches.SetPoint(cp, row, col);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistry->RegisterControlPoint(cylinder, cp, Coordinator::GetSystemID<C0PatchesSystem>());
        }
    }

    for (size_t row=0; row < patches.PointsInRow(); ++row) {
        const Entity cp = patches.GetPoint(row, 0);
        patches.SetPoint(cp, row, patches.PointsInCol() - 1);
    }

    const Mesh mesh;
    const PatchesDensity density(5);

    patches.deletionHandler = coordinator->Subscribe<C0Patches>(cylinder, deletionHandler);

    coordinator->AddComponent<Mesh>(cylinder, mesh);
    coordinator->AddComponent<C0Patches>(cylinder, patches);
    coordinator->AddComponent<PatchesDensity>(cylinder, density);
    coordinator->AddComponent<WrapV>(cylinder, WrapV());

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    RecalculateCylinder(cylinder, pos, direction, radius);

    return cylinder;
}


void C0PatchesSystem::AddRowOfCylinderPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius
) const {
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddRowOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (size_t col=0; col < patches.PointsInCol() - 1; col++) {
                for (size_t row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint();

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    cpRegistrySys->RegisterControlPoint(cylinder, newEntity, Coordinator::GetSystemID<C0PatchesSystem>());
                }
            }

            for (size_t row = patches.PointsInRow() - 3; row < patches.PointsInRow(); ++row) {
                const Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    RecalculateCylinder(cylinder, pos, direction, radius);
}


void C0PatchesSystem::AddColOfCylinderPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, const float radius
) const {
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddColOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (size_t row=0; row < patches.PointsInRow(); ++row) {
                const Entity cp = patches.GetPoint(row, 0);
                patches.SetPoint(cp, row, patches.PointsInCol() - 1);
            }

            for (size_t row=0; row < patches.PointsInRow(); row++) {
                for (size_t col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
                    Entity newEntity = pointSys->CreatePoint();

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    cpRegistrySys->RegisterControlPoint(cylinder, newEntity, Coordinator::GetSystemID<C0PatchesSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    RecalculateCylinder(cylinder, pos, direction, radius);
}


void C0PatchesSystem::DeleteRowOfCylinderPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius
) const {
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (size_t col=0; col < patches.PointsInCol() - 1; col++) {
                for (size_t row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(cylinder, point, Coordinator::GetSystemID<C0PatchesSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);
    RecalculateCylinder(cylinder, pos, direction, radius);
}


void C0PatchesSystem::DeleteColOfCylinderPatches(Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius
) const {
    coordinator->EditComponent<C0Patches>(cylinder,
        [cylinder, this](C0Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (size_t row=0; row < patches.PointsInRow(); row++) {
                for (size_t col=patches.PointsInCol() - 4; col < patches.PointsInCol() - 1; col++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(cylinder, point, Coordinator::GetSystemID<C0PatchesSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            for (size_t row=0; row < patches.PointsInRow(); row++) {
                const Entity point = patches.GetPoint(row, patches.PointsInCol() - 1);
                patches.SetPoint(point, row, patches.PointsInCol() - 4);
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(cylinder);

    RecalculateCylinder(cylinder, pos, direction, radius);
}


void C0PatchesSystem::RecalculateCylinder(const Entity cylinder, const Position &pos, const alg::Vec3 &direction, float radius
) const {
    auto const& patches = coordinator->GetComponent<C0Patches>(cylinder);

    const alg::Vec3 offset = direction / static_cast<float>(patches.PointsInRow());

    const Rotation rot(direction, 2.f * std::numbers::pi_v<float> / static_cast<float>(patches.PointsInCol() - 1));

    alg::Vec3 v = GetPerpendicularVec(direction) * radius + pos.vec;

    for (size_t row=0; row < patches.PointsInRow(); ++row) {
        for (size_t col=0; col < patches.PointsInCol() - 1; ++col) {
            const Entity cp = patches.GetPoint(row, col);
            coordinator->SetComponent(cp, Position(v));

            rot.Rotate(v);
        }

        v += offset;
    }
}


Entity C0PatchesSystem::CreatePlane(const Position &pos, const alg::Vec3 &direction, float length, float width)
{
    C0Patches patches(1, 1);

    auto const pointsSystem = coordinator->GetSystem<PointsSystem>();
    const auto cpRegistrySystem = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (size_t col=0; col < patches.PointsInCol(); ++col) {
        for (size_t row=0; row < patches.PointsInRow(); ++row) {
            // Creating control points with temporary location
            Entity cp = pointsSystem->CreatePoint(pos.vec);
            patches.SetPoint(cp, col, row);

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistrySystem->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C0PatchesSystem>());
        }
    }

    patches.deletionHandler = coordinator->Subscribe<C0Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, Mesh());
    coordinator->AddComponent<C0Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, PatchesDensity(5));

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);

    return surface;
}


Entity C0PatchesSystem::CreateSurface(C0Patches &patches)
{
    const auto cpRegistrySystem = coordinator->GetSystem<ControlPointsRegistrySystem>();

    Entity surface = coordinator->CreateEntity();

    const auto handler = std::make_shared<ControlPointMovedHandler>(surface, *coordinator);

    for (size_t col=0; col < patches.PointsInCol(); ++col) {
        for (size_t row=0; row < patches.PointsInRow(); ++row) {
            // Creating control points with temporary location
            Entity cp = patches.GetPoint(row, col);

            if (patches.controlPointsHandlers.contains(cp))
                continue;

            HandlerId cpHandler = coordinator->Subscribe(cp, std::static_pointer_cast<EventHandler<Position>>(handler));
            patches.controlPointsHandlers.insert({cp, cpHandler});

            cpRegistrySystem->RegisterControlPoint(surface, cp, Coordinator::GetSystemID<C0PatchesSystem>());
        }
    }

    patches.deletionHandler = coordinator->Subscribe<C0Patches>(surface, deletionHandler);

    coordinator->AddComponent<Mesh>(surface, Mesh());
    coordinator->AddComponent<C0Patches>(surface, patches);
    coordinator->AddComponent<PatchesDensity>(surface, PatchesDensity(5));

    if (ShouldWrapU(patches))
        coordinator->AddComponent(surface, WrapU());

    if (ShouldWrapV(patches))
        coordinator->AddComponent(surface, WrapV());

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    return surface;
}


void C0PatchesSystem::AddRowOfPlanePatches(Entity surface, const Position &pos, const alg::Vec3 &direction,
    const float length, const float width
) const {
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddRowOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (size_t col=0; col < patches.PointsInCol(); col++) {
                for (size_t row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C0PatchesSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C0PatchesSystem::AddColOfPlanePatches(Entity surface, const Position &pos, const alg::Vec3 &direction,
    const float length, const float width
) const {
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            const auto pointSys = coordinator->GetSystem<PointsSystem>();
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            patches.AddColOfPatches();

            const Entity firstCP = patches.GetPoint(0,0);
            const HandlerId firstCpHandler = patches.controlPointsHandlers.at(firstCP);
            const auto eventHandler = coordinator->GetEventHandler<Position>(firstCP, firstCpHandler);

            for (size_t row=0; row < patches.PointsInRow(); row++) {
                for (size_t col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity newEntity = pointSys->CreatePoint(Position());

                    patches.SetPoint(newEntity, row, col);

                    HandlerId newHandler = coordinator->Subscribe<Position>(newEntity, eventHandler);
                    patches.controlPointsHandlers.insert({ newEntity, newHandler });

                    cpRegistrySys->RegisterControlPoint(surface, newEntity, Coordinator::GetSystemID<C0PatchesSystem>());
                }
            }
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C0PatchesSystem::DeleteRowOfPlanePatches(Entity surface, const Position &pos, const alg::Vec3 &direction,
    const float length, const float width
) const {
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (size_t col=0; col < patches.PointsInCol(); col++) {
                for (size_t row=patches.PointsInRow() - 3; row < patches.PointsInRow(); row++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C0PatchesSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteRowOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C0PatchesSystem::DeleteColOfPlanePatches(Entity surface, const Position &pos, const alg::Vec3 &direction,
    const float length, const float width
) const {
    coordinator->EditComponent<C0Patches>(surface,
        [surface, this](C0Patches& patches) {
            const auto cpRegistrySys = coordinator->GetSystem<ControlPointsRegistrySystem>();

            for (size_t row=0; row < patches.PointsInRow(); row++) {
                for (size_t col=patches.PointsInCol() - 3; col < patches.PointsInCol(); col++) {
                    Entity point = patches.GetPoint(row, col);
                    cpRegistrySys->UnregisterControlPoint(surface, point, Coordinator::GetSystemID<C0PatchesSystem>());
                    coordinator->DestroyEntity(point);

                    patches.controlPointsHandlers.erase(point);
                }
            }

            patches.DeleteColOfPatches();
        }
    );

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    RecalculatePlane(surface, pos, direction, length, width);
}


void C0PatchesSystem::RecalculatePlane(const Entity surface, const Position &pos, const alg::Vec3 &direction,
    const float length, const float width
) const {
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);

    alg::Vec3 perpendicular1 = GetPerpendicularVec(direction);
    alg::Vec3 perpendicular2 = Cross(perpendicular1, direction);

    perpendicular1 *= length / static_cast<float>(patches.PointsInRow() - 1);
    perpendicular2 *= width / static_cast<float>(patches.PointsInCol() - 1);

    for (size_t row=0; row < patches.PointsInRow(); ++row) {
        for (size_t col=0; col < patches.PointsInCol(); ++col) {
            const Entity cp = patches.GetPoint(row, col);

            alg::Vec3 newPos = pos.vec + perpendicular1 * static_cast<float>(row) + perpendicular2 * static_cast<float>(col);

            coordinator->SetComponent(cp, Position(newPos));
        }
    }
}


void C0PatchesSystem::MergeControlPoints(const Entity surface, const Entity oldCP, Entity newCP, const SystemId system)
{
    if (oldCP == newCP)
        return;

    coordinator->EditComponent<C0Patches>(surface,
        [oldCP, newCP, this] (C0Patches& patches) {
            for (size_t row=0; row < patches.PointsInRow(); row++) {
                for (size_t col=0; col < patches.PointsInCol(); col++) {
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

    coordinator->GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(surface);

    const auto registry = coordinator->GetSystem<ControlPointsRegistrySystem>();
    registry->UnregisterControlPoint(surface, oldCP, system);
    registry->RegisterControlPoint(surface, newCP, system);

    // Update wrapping
    const auto& c0Patches = coordinator->GetComponent<C0Patches>(surface);

    bool hasWrapping = coordinator->HasComponent<WrapU>(surface);
    bool shouldWrap = ShouldWrapU(c0Patches);

    if (hasWrapping != shouldWrap) {
        if (hasWrapping)
            coordinator->DeleteComponent<WrapU>(surface);
        else
            coordinator->AddComponent<WrapU>(surface, WrapU());
    }

    hasWrapping = coordinator->HasComponent<WrapV>(surface);
    shouldWrap = ShouldWrapV(c0Patches);

    if (hasWrapping != shouldWrap) {
        if (hasWrapping)
            coordinator->DeleteComponent<WrapV>(surface);
        else
            coordinator->AddComponent<WrapV>(surface, WrapV());
    }
}


void C0PatchesSystem::ShowBezierNet(const Entity surface)
{
    auto const& patches = coordinator->GetComponent<C0Patches>(surface);
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    netSystem->AddControlPointsNet(surface, patches);
}


alg::Vec4 CubicBernsteinPolynomial(const float t) {
    const float oneMinusT = 1.0f - t;

    return {
        oneMinusT * oneMinusT * oneMinusT,
        3.f * oneMinusT * oneMinusT * t,
        3.f * oneMinusT * t * t,
        t * t * t
    };
}


Position C0PatchesSystem::PointOnSurface(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bu = CubicBernsteinPolynomial(u);
    const auto bv = CubicBernsteinPolynomial(v);

    // Result of bu*p
    const auto a0 = bu.X()*p.Point(0, 0) + bu.Y()*p.Point(1, 0) + bu.Z()*p.Point(2, 0) + bu.W()*p.Point(3, 0);
    const auto a1 = bu.X()*p.Point(0, 1) + bu.Y()*p.Point(1, 1) + bu.Z()*p.Point(2, 1) + bu.W()*p.Point(3, 1);
    const auto a2 = bu.X()*p.Point(0, 2) + bu.Y()*p.Point(1, 2) + bu.Z()*p.Point(2, 2) + bu.W()*p.Point(3, 2);
    const auto a3 = bu.X()*p.Point(0, 3) + bu.Y()*p.Point(1, 3) + bu.Z()*p.Point(2, 3) + bu.W()*p.Point(3, 3);

    // pos = bu * p * bv^T
    return a0*bv.X() + a1*bv.Y() + a2*bv.Z() + a3*bv.W();
}


alg::Vec3 Derivative(const float t, const alg::Vec3& a0, const alg::Vec3& a1, const alg::Vec3& a2, const alg::Vec3& a3) {
    const float oneMinusT = 1.f - t;

    return -3.f * oneMinusT * oneMinusT * a0 +
           (3.f * oneMinusT * oneMinusT - 6.f * t * oneMinusT) * a1 +
           (6.f * t * oneMinusT - 3.f * t * t) * a2 +
           3.f * t * t * a3;
}


alg::Vec3 C0PatchesSystem::PartialDerivativeU(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bv = CubicBernsteinPolynomial(v);

    // Result of bu*p
    const auto a0 = bv.X()*p.Point(0, 0) + bv.Y()*p.Point(0, 1) + bv.Z()*p.Point(0, 2) + bv.W()*p.Point(0, 3);
    const auto a1 = bv.X()*p.Point(1, 0) + bv.Y()*p.Point(1, 1) + bv.Z()*p.Point(1, 2) + bv.W()*p.Point(1, 3);
    const auto a2 = bv.X()*p.Point(2, 0) + bv.Y()*p.Point(2, 1) + bv.Z()*p.Point(2, 2) + bv.W()*p.Point(2, 3);
    const auto a3 = bv.X()*p.Point(3, 0) + bv.Y()*p.Point(3, 1) + bv.Z()*p.Point(3, 2) + bv.W()*p.Point(3, 3);

    return Derivative(u, a0, a1, a2, a3);
}


alg::Vec3 C0PatchesSystem::PartialDerivativeV(const C0Patches &patches, float u, float v) const
{
    CheckUVDomain(patches, u, v);

    const SingleC0Patch p(*coordinator, patches, u, v);

    NormalizeUV(patches, u, v);

    const auto bu = CubicBernsteinPolynomial(u);

    // Result of bu*p
    const auto a0 = bu.X()*p.Point(0, 0) + bu.Y()*p.Point(1, 0) + bu.Z()*p.Point(2, 0) + bu.W()*p.Point(3, 0);
    const auto a1 = bu.X()*p.Point(0, 1) + bu.Y()*p.Point(1, 1) + bu.Z()*p.Point(2, 1) + bu.W()*p.Point(3, 1);
    const auto a2 = bu.X()*p.Point(0, 2) + bu.Y()*p.Point(1, 2) + bu.Z()*p.Point(2, 2) + bu.W()*p.Point(3, 2);
    const auto a3 = bu.X()*p.Point(0, 3) + bu.Y()*p.Point(1, 3) + bu.Z()*p.Point(2, 3) + bu.W()*p.Point(3, 3);

    return Derivative(v, a0, a1, a2, a3);
}


void C0PatchesSystem::Update() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();
    auto const& netSystem = coordinator->GetSystem<ControlNetSystem>();

    for (const auto entity: toUpdateSystem->GetEntitiesToUpdate<C0PatchesSystem>()) {
        auto const& patches = coordinator->GetComponent<C0Patches>(entity);

        UpdateMesh(entity, patches);

        if (netSystem->HasControlPointsNet(entity))
            netSystem->Update(entity, patches);
    }

    toUpdateSystem->UnmarkAll<C0PatchesSystem>();
}


void C0PatchesSystem::UpdateMesh(const Entity surface, const C0Patches& patches) const
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


void C0PatchesSystem::CheckUVDomain(const C0Patches &patches, const float u, const float v)
{
    if (!(v >= 0.f && v <= MaxV(patches)) || !(u >= 0.f && u <= MaxU(patches)))
        throw std::runtime_error("Arguments outside the domain");
}


void C0PatchesSystem::NormalizeUV(const C0Patches &patches, float& u, float& v)
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


bool C0PatchesSystem::ShouldWrapU(const C0Patches& patches)
{
    for (size_t col=0; col<patches.PointsInCol(); ++col) {
        if (patches.GetPoint(0, col) != patches.GetPoint(patches.PointsInRow() - 1, col))
            return false;
    }

    return true;
}


bool C0PatchesSystem::ShouldWrapV(const C0Patches &patches)
{
    for (size_t row=0; row<patches.PointsInRow(); ++row) {
        if (patches.GetPoint(row, 0) != patches.GetPoint(row, patches.PointsInCol() - 1))
            return false;
    }

    return true;
}


std::vector<float> C0PatchesSystem::GenerateVertices(const C0Patches& patches) const
{
    std::vector<float> result;
    result.reserve(patches.PointsCnt() * 3);

    for (size_t col=0; col < patches.PointsInCol(); col++) {
        for (size_t row=0; row < patches.PointsInRow(); row++) {
            const Entity point = patches.GetPoint(row, col);

            auto const& pos = coordinator->GetComponent<Position>(point);

            result.push_back(pos.GetX());
            result.push_back(pos.GetY());
            result.push_back(pos.GetZ());
        }
    }

    return result;
}


std::vector<uint32_t> C0PatchesSystem::GenerateIndices(const C0Patches& patches) const
{
    std::vector<uint32_t> result;
    result.reserve(patches.PatchesInRow() * patches.PatchesInCol() * C0Patches::PointsInPatch * 2);

    for (int patchRow=0; patchRow < patches.PatchesInRow(); patchRow++) {
        for (int patchCol=0; patchCol < patches.PatchesInCol(); patchCol++) {
            for (int rowInPatch=0; rowInPatch < C0Patches::RowsInPatch; rowInPatch++) {
                for (int colInPatch=0; colInPatch < C0Patches::ColsInPatch; colInPatch++) {

                    const int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    const int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }

            for (int colInPatch=0; colInPatch < C0Patches::ColsInPatch; colInPatch++) {
                for (int rowInPatch=0; rowInPatch < C0Patches::RowsInPatch; rowInPatch++) {

                    const int globCol = patchCol * (C0Patches::ColsInPatch - 1) + colInPatch;
                    const int globRow = patchRow * (C0Patches::RowsInPatch - 1) + rowInPatch;

                    result.push_back(globCol * patches.PointsInRow() + globRow);
                }
            }
        }
    }

    return result;
}


void C0PatchesSystem::DeletionHandler::HandleEvent(const Entity entity, const C0Patches &component, const EventType eventType)
{
    if (eventType != EventType::ComponentDeleted)
        return;

    const auto cpRegistry = coordinator.GetSystem<ControlPointsRegistrySystem>();

    for (const auto [cp, handlerId]: component.controlPointsHandlers) {
        coordinator.Unsubscribe<Position>(cp, handlerId);
        cpRegistry->UnregisterControlPoint(entity, cp, Coordinator::GetSystemID<C0PatchesSystem>());

        if (!cpRegistry->IsAControlPoint(cp))
            coordinator.DestroyEntity(cp);
    }
}


void C0PatchesSystem::ControlPointMovedHandler::HandleEvent(const Entity entity, const Position &component, const EventType eventType
) {
    (void)entity;
    (void)component;
    (void)eventType;

    coordinator.GetSystem<ToUpdateSystem>()->MarkAsToUpdate<C0PatchesSystem>(targetObject);
}
