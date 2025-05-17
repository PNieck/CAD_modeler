#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/wraps.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <numbers>


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

    const Scale scale;
    coordinator->AddComponent<Scale>(torus, scale);

    const Rotation rot;
    coordinator->AddComponent<Rotation>(torus, rot);

    coordinator->AddComponent<TorusParameters>(torus, params);

    coordinator->AddComponent<Mesh>(torus, Mesh());
    UpdateMesh(torus, params);

    coordinator->AddComponent(torus, WrapU());
    coordinator->AddComponent(torus, WrapV());

    return torus;
}


void ToriSystem::SetParameters(const Entity entity, const TorusParameters& params)
{
    coordinator->SetComponent<TorusParameters>(entity, params);
    UpdateMesh(entity, params);
}


void ToriSystem::Render(const alg::Mat4x4& cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);

        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        alg::Mat4x4 modelMtx = position.TranslationMatrix() * rotation.GetRotationMatrix() * scale.ScaleMatrix();
        shader.SetMVP(cameraMtx * modelMtx);
        
        mesh.Use();
        glDrawElements(GL_LINE_LOOP, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);
        
        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


Position ToriSystem::PointOnSurface(Entity torus, float u, float v) const
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


void ToriSystem::UpdateMesh(const Entity e, const TorusParameters &params) const
{
    coordinator->EditComponent<Mesh>(e,
        [this, &params] (Mesh& mesh) {
            mesh.Update(
                GenerateMeshVertices(params),
                GenerateMeshIndices(params)
            );
        }
    );
}


std::vector<float> ToriSystem::GenerateMeshVertices(const TorusParameters &params)
{
    const float deltaU = 2.f * std::numbers::pi_v<float> / params.meshDensityMinR;
    const float deltaV = 2.f * std::numbers::pi_v<float> / params.meshDensityMajR;

    std::vector<float> result(params.meshDensityMinR * params.meshDensityMajR * 3);

    for (int actCirc = 0; actCirc < params.meshDensityMajR; actCirc++) {
        const float v = deltaV * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < params.meshDensityMinR; act_pt_in_circ++) {
            int index = (actCirc * params.meshDensityMinR + act_pt_in_circ) * 3;

            float u = deltaU * act_pt_in_circ;

            auto point = PointOnSurface(params, u, v);

            result[index] = point.X();
            result[index + 1] = point.Y();
            result[index + 2] = point.Z();
        }
    }

    return result;
}


std::vector<uint32_t> ToriSystem::GenerateMeshIndices(const TorusParameters &params)
{
    std::vector<uint32_t> result(
        params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR + params.meshDensityMinR*params.meshDensityMajR + params.meshDensityMinR
    );

    for (int i = 0; i < params.meshDensityMajR; i++) {
        for (int j=0; j < params.meshDensityMinR; j++) {
            result[i*params.meshDensityMinR + j + i] = j + i*params.meshDensityMinR;
        }

        result[params.meshDensityMinR * (i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    const uint32_t index = params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR;

    for (int i = 0; i < params.meshDensityMinR; i++) {
        for (int j=0; j < params.meshDensityMajR; j++) {
            result[index + j + i*params.meshDensityMajR + i] = j * params.meshDensityMinR + i;
        }

        result[index + params.meshDensityMajR*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    return result;
}
