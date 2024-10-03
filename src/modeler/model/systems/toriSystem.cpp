#include <CAD_modeler/model/systems/toriSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/mesh.hpp>
#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <numbers>


void ToriSystem::RegisterSystem(Coordinator & coordinator)
{
    coordinator.RegisterSystem<ToriSystem>();

    coordinator.RegisterRequiredComponent<ToriSystem, Position>();
    coordinator.RegisterRequiredComponent<ToriSystem, Scale>();
    coordinator.RegisterRequiredComponent<ToriSystem, Rotation>();
    coordinator.RegisterRequiredComponent<ToriSystem, Mesh>();
    coordinator.RegisterRequiredComponent<ToriSystem, TorusParameters>();
}


void ToriSystem::Init(ShaderRepository * shaderRepo)
{
    this->shadersRepo = shaderRepo;
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
    coordinator->AddComponent<Name>(torus, nameGenerator.GenerateName("Torus"));

    const auto vertices = GenerateMeshVertices(params);
    const auto indices = GenerateMeshIndices(params);

    Mesh mesh;
    mesh.Update(vertices, indices);
    coordinator->AddComponent<Mesh>(torus, mesh);

    return torus;
}


void ToriSystem::SetParameters(Entity entity, const TorusParameters& params)
{
    coordinator->SetComponent<TorusParameters>(entity, params);

    auto vertices = GenerateMeshVertices(params);
    auto indices = GenerateMeshIndices(params);

    Mesh mesh = coordinator->GetComponent<Mesh>(entity);
    mesh.Update(vertices, indices);
    coordinator->SetComponent<Mesh>(entity, mesh);
}


void ToriSystem::Render(const alg::Mat4x4& cameraMtx)
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = shadersRepo->GetStdShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));

    for (auto const entity : entities) {
        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        auto const& position = coordinator->GetComponent<Position>(entity);
        auto const& scale = coordinator->GetComponent<Scale>(entity);
        auto const& rotation = coordinator->GetComponent<Rotation>(entity);
        
        bool selection = selectionSystem->IsSelected(entity);

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


Position ToriSystem::PointOnTorus(Entity torus, float alpha, float beta) const
{
    return PointOnTorus(
        coordinator->GetComponent<TorusParameters>(torus),
        coordinator->GetComponent<Position>(torus),
        coordinator->GetComponent<Rotation>(torus),
        coordinator->GetComponent<Scale>(torus),
        alpha,
        beta
    );
}


alg::Vec3 ToriSystem::PointOnTorus(const TorusParameters &params, float alpha, float beta) {
    return {
        (params.majorRadius + params.minorRadius * std::cos(alpha)) * std::cos(beta),
        params.minorRadius * std::sin(alpha),
        (params.majorRadius + params.minorRadius * std::cos(alpha)) * std::sin(beta)
    };
}


Position ToriSystem::PointOnTorus(const TorusParameters &params, const Position &pos, const Rotation &rot, const Scale &scale, float alpha, float beta) const
{
    alg::Vec3 result = PointOnTorus(params, alpha, beta);

    scale.TransformVector(result);
    rot.Rotate(result);

    return result + pos.vec;
}


alg::Vec3 ToriSystem::PartialDerivativeWithRespectToAlpha(const TorusParameters &params, const Rotation &rot,
    const Scale &scale, const float alpha, const float beta) {
    alg::Vec3 result(
        -params.minorRadius * std::cos(beta) * std::sin(alpha),
        params.minorRadius * std::cos(alpha),
        -params.minorRadius * std::sin(beta) * std::sin(alpha)
    );

    scale.TransformVector(result);
    rot.Rotate(result);

    return result;
}


alg::Vec3 ToriSystem::PartialDerivativeWithRespectToAlpha(Entity e, float alpha, float beta) const
{
    return PartialDerivativeWithRespectToAlpha(
        coordinator->GetComponent<TorusParameters>(e),
        coordinator->GetComponent<Rotation>(e),
        coordinator->GetComponent<Scale>(e),
        alpha,
        beta
    );
}


alg::Vec3 ToriSystem::PartialDerivativeWithRespectToBeta(const TorusParameters &params, const Rotation &rot,
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


alg::Vec3 ToriSystem::PartialDerivativeWithRespectToBeta(Entity e, float alpha, float beta) const
{
    return PartialDerivativeWithRespectToBeta(
        coordinator->GetComponent<TorusParameters>(e),
        coordinator->GetComponent<Rotation>(e),
        coordinator->GetComponent<Scale>(e),
        alpha,
        beta
    );
}


std::vector<float> ToriSystem::GenerateMeshVertices(const TorusParameters &params) const
{
    float deltaAlpha = 2.f * std::numbers::pi_v<float> / params.meshDensityMinR;
    float deltaBeta = 2.f * std::numbers::pi_v<float> / params.meshDensityMajR;

    std::vector<float> result(params.meshDensityMinR * params.meshDensityMajR * 3);

    for (int actCirc = 0; actCirc < params.meshDensityMajR; actCirc++) {
        const float beta = deltaBeta * actCirc;

        for (int act_pt_in_circ = 0; act_pt_in_circ < params.meshDensityMinR; act_pt_in_circ++) {
            int index = (actCirc * params.meshDensityMinR + act_pt_in_circ) * 3;

            float alpha = deltaAlpha * act_pt_in_circ;

            auto point = PointOnTorus(params, alpha, beta);

            result[index] = point.X();
            result[index + 1] = point.Y();
            result[index + 2] = point.Z();
        }
    }

    return result;
}


std::vector<uint32_t> ToriSystem::GenerateMeshIndices(const TorusParameters &params) const
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

    uint32_t index = params.meshDensityMinR * params.meshDensityMajR + params.meshDensityMajR;

    for (int i = 0; i < params.meshDensityMinR; i++) {
        for (int j=0; j < params.meshDensityMajR; j++) {
            result[index + j + i*params.meshDensityMajR + i] = j * params.meshDensityMinR + i;
        }

        result[index + params.meshDensityMajR*(i+1) + i] = std::numeric_limits<uint32_t>::max();
    }

    return result;
}
