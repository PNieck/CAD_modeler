#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/wraps.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/toriRenderingSystem.hpp>


void ToriSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ToriSystem>();

    coordinator.RegisterComponent<WrapU>();
    coordinator.RegisterComponent<WrapV>();

    coordinator.RegisterRequiredComponent<ToriSystem, Position>();
    coordinator.RegisterRequiredComponent<ToriSystem, Scale>();
    coordinator.RegisterRequiredComponent<ToriSystem, Rotation>();
    coordinator.RegisterRequiredComponent<ToriSystem, Mesh>();
    coordinator.RegisterRequiredComponent<ToriSystem, TorusParameters>();
}


Entity ToriSystem::AddTorus(const Position& pos, const TorusParameters& params)
{
    const Entity torus = coordinator->CreateEntity();

    coordinator->AddComponent<Position>(torus, pos);
    coordinator->AddComponent<Scale>(torus, Scale());
    coordinator->AddComponent<Rotation>(torus, Rotation());
    coordinator->AddComponent<TorusParameters>(torus, params);
    coordinator->AddComponent<Mesh>(torus, Mesh());

    coordinator->AddComponent(torus, WrapU());
    coordinator->AddComponent(torus, WrapV());

    return torus;
}


void ToriSystem::SetParameters(const Entity entity, const TorusParameters& params)
{
    coordinator->SetComponent<TorusParameters>(entity, params);
}


Position ToriSystem::PointOnSurface(const Entity torus, const float u, const float v) const
{
    return PointOnSurface(
        coordinator->GetComponent<TorusParameters>(torus),
        coordinator->GetComponent<Position>(torus),
        coordinator->GetComponent<Rotation>(torus),
        coordinator->GetComponent<Scale>(torus),
        u,
        v
    );
}


alg::Vec3 ToriSystem::PointOnSurface(const TorusParameters &params, float u, float v) {
    return {
        (params.majorRadius + params.minorRadius * std::cos(u)) * std::cos(v),
        params.minorRadius * std::sin(u),
        (params.majorRadius + params.minorRadius * std::cos(u)) * std::sin(v)
    };
}


Position ToriSystem::PointOnSurface(const TorusParameters &params, const Position &pos, const Rotation &rot, const Scale &scale, float u, float v)
{
    alg::Vec3 result = PointOnSurface(params, u, v);

    scale.TransformVector(result);
    rot.Rotate(result);

    return result + pos.vec;
}


alg::Vec3 ToriSystem::PartialDerivativeU(const TorusParameters &params, const Rotation &rot,
    const Scale &scale, const float u, const float v) {
    alg::Vec3 result(
        -params.minorRadius * std::cos(v) * std::sin(u),
        params.minorRadius * std::cos(u),
        -params.minorRadius * std::sin(v) * std::sin(u)
    );

    scale.TransformVector(result);
    rot.Rotate(result);

    return result;
}


alg::Vec3 ToriSystem::PartialDerivativeU(const Entity e, const float u, const float v) const
{
    return PartialDerivativeU(
        coordinator->GetComponent<TorusParameters>(e),
        coordinator->GetComponent<Rotation>(e),
        coordinator->GetComponent<Scale>(e),
        u,
        v
    );
}


alg::Vec3 ToriSystem::PartialDerivativeV(const TorusParameters &params, const Rotation &rot,
                                                         const Scale &scale, const float alpha, const float beta) {
    alg::Vec3 result (
        -(params.minorRadius * std::cos(alpha) + params.majorRadius) * std::sin(beta),
        0.f,
        (params.minorRadius * std::cos(alpha) + params.majorRadius) * std::cos(beta)
    );

    scale.TransformVector(result);
    rot.Rotate(result);

    return result;
}


alg::Vec3 ToriSystem::PartialDerivativeV(Entity e, float u, float v) const
{
    return PartialDerivativeV(
        coordinator->GetComponent<TorusParameters>(e),
        coordinator->GetComponent<Rotation>(e),
        coordinator->GetComponent<Scale>(e),
        u,
        v
    );
}


alg::Vec3 ToriSystem::NormalVector(const TorusParameters &params, const Rotation &rot, const Scale &scale, const float alpha,
                                const float beta)
{
    const alg::Vec3 tangent1 = PartialDerivativeU(params, rot, scale, alpha, beta);
    const alg::Vec3 tangent2 = PartialDerivativeV(params, rot, scale, alpha, beta);

    const alg::Vec3 result = Cross(tangent1, tangent2);

    return result.Normalize();
}


alg::Vec3 ToriSystem::NormalVector(const Entity e, const float u, const float v) const
{
    return NormalVector(
        coordinator->GetComponent<TorusParameters>(e),
        coordinator->GetComponent<Rotation>(e),
        coordinator->GetComponent<Scale>(e),
        u,
        v
    );
}
