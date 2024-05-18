#include <CAD_modeler/model/systems/interpolationCurveSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/components/name.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/cameraSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>

#include <CAD_modeler/utilities/setIntersection.hpp>

#include <algebra/systemsOfLinearEquasions.hpp>
#include <algebra/cubicPolynomials.hpp>

#include <cassert>


void InterpolationCurveSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<InterpolationCurveSystem>();
}


Entity InterpolationCurveSystem::CreateCurve(const std::vector<Entity> &controlPoints)
{
    Entity curve = coordinator->GetSystem<ControlPointsSystem>()->CreateControlPoints(controlPoints);

    coordinator->AddComponent<Name>(curve, nameGenerator.GenerateName("InterpolationCurve"));
    coordinator->AddComponent<Mesh>(curve, Mesh());

    UpdateMesh(curve);

    entities.insert(curve);

    return curve;
}


void InterpolationCurveSystem::Render() const
{
    if (entities.empty()) {
        return;
    }

    auto const& cameraSystem = coordinator->GetSystem<CameraSystem>();
    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();

    auto const& shader = shaderRepo->GetBezierCurveShader();

    UpdateEntities();

    alg::Mat4x4 cameraMtx = cameraSystem->PerspectiveMatrix() * cameraSystem->ViewMatrix();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        auto const& controlPoints = coordinator->GetComponent<ControlPoints>(entity);

        bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 4);
	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, 0);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void InterpolationCurveSystem::UpdateEntities() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    auto toUpdate = intersect(toUpdateSystem->GetEntities(), entities);

    for (auto entity: toUpdate) {
        UpdateMesh(entity);
    }
}


void InterpolationCurveSystem::UpdateMesh(Entity entity) const
{
    coordinator->EditComponent<Mesh>(entity,
        [this, entity](Mesh& mesh) {
            auto const cps = coordinator->GetComponent<ControlPoints>(entity);

            mesh.Update(
                GenerateMeshVertices(cps),
                GenerateMeshIndices(cps)
            );
        }
    );
}


std::vector<float> InterpolationCurveSystem::GenerateMeshVertices(const ControlPoints &cps) const
{
    std::vector<float> result;

    if (cps.Size() < 2)
        return result;

    auto powerBasis = FindInterpolationsPolynomialsInPowerBasis(cps);
    result.reserve(powerBasis.size() * 4 * 3);

    for (auto const& polynomial: powerBasis) {
        auto bernsteinBasis = alg::FromPowerToBernsteinBasis(polynomial);

        auto const& polynomial1 = bernsteinBasis.X();

        result.push_back(polynomial1.X());
        result.push_back(polynomial1.Y());
        result.push_back(polynomial1.Z());
        
        auto const& polynomial2 = bernsteinBasis.Y();

        result.push_back(polynomial2.X());
        result.push_back(polynomial2.Y());
        result.push_back(polynomial2.Z());

        auto const& polynomial3 = bernsteinBasis.Z();

        result.push_back(polynomial3.X());
        result.push_back(polynomial3.Y());
        result.push_back(polynomial3.Z());

        auto const& polynomial4 = bernsteinBasis.W();

        result.push_back(polynomial4.X());
        result.push_back(polynomial4.Y());
        result.push_back(polynomial4.Z());
    }

    return result;
}


std::vector<uint32_t> InterpolationCurveSystem::GenerateMeshIndices(const ControlPoints &cps) const
{
    std::vector<uint32_t> result((cps.Size() - 1) * 4);

    for (int i=0; i < result.size(); ++i) {
        result[i] = i;
    }

    return result;
}


std::vector<alg::Vector4<alg::Vec3>> InterpolationCurveSystem::FindInterpolationsPolynomialsInPowerBasis(const ControlPoints& cps) const
{
    auto chordLengths = ChordLengthsBetweenControlPoints(cps);

    auto squareCoeff = SquareCoeffInPowerBasis(cps, chordLengths);
    auto cubicCoeff = CubicCoeffInPowerBasis(squareCoeff, chordLengths);
    auto linearCoeff = LinearCoeffInPowerBasis(squareCoeff, cubicCoeff, chordLengths, cps);
    auto constCoeff = ConstantCoeffInPowerBasis(linearCoeff, squareCoeff, cubicCoeff, chordLengths, cps);

    std::vector<alg::Vector4<alg::Vec3>> result(constCoeff.size());

    for (int i=0; i < constCoeff.size(); ++i) {
        result[i] = alg::Vector4(
            constCoeff[i],
            linearCoeff[i],
            squareCoeff[i],
            cubicCoeff[i]
        );
    }

    for (int i=0; i < result.size(); ++i) {
        ChangePolynomialInPowerBaseDomainFrom0To1(result[i], chordLengths[i]);
    }

    return result;
}


void InterpolationCurveSystem::ChangePolynomialInPowerBaseDomainFrom0To1(alg::Vector4<alg::Vec3> &polynomial, float domain) const
{
    polynomial.Y() *= domain;
    polynomial.Z() *= domain*domain;
    polynomial.W() *= domain*domain*domain;
}


std::vector<alg::Vec3> InterpolationCurveSystem::SquareCoeffInPowerBasis(const ControlPoints& cps, const std::vector<float>& chordsLen) const
{
    if (cps.Size() == 2) {
        return { alg::Vec3(0), alg::Vec3(0) };
    }

    auto superDiagonal = SuperdiagonalElems(chordsLen);
    auto diagonal = DiagonalElems(chordsLen);
    auto subdiagonal = SubdiagonalElems(chordsLen);
    auto equationResults = EquationResultsElems(chordsLen, cps);

    auto coeff = alg::SolveSystemOfLinearEquationsWithTridiagonalMtx(
        superDiagonal,
        diagonal,
        subdiagonal,
        equationResults
    );

    assert(coeff.has_value());

    std::vector<alg::Vec3> result(coeff.value().size() + 2);

    result[0] = alg::Vec3(0);

    for (int i=1; i <= result.size() - 2; ++i) {
        result[i] = coeff.value()[i-1];
    }

    result[result.size() - 1] = alg::Vec3(0);

    return result;
}


std::vector<alg::Vec3> InterpolationCurveSystem::CubicCoeffInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoeff, const std::vector<float>& chordsLen) const
{
    std::vector<alg::Vec3> result(squarePowerCoeff.size() - 1);

    for (int i=0; i < result.size(); ++i) {
        result[i] = (squarePowerCoeff[i+1] - squarePowerCoeff[i]) / (3.f * chordsLen[i]);
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurveSystem::LinearCoeffInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoeff, const std::vector<alg::Vec3> cubicPowerCoeff, const std::vector<float>& chordsLen, const ControlPoints &cps) const
{
    auto const& controlPoints = cps.GetPoints();

    std::vector<alg::Vec3> result(squarePowerCoeff.size() - 1);

    float t = chordsLen[0];

    auto const& cpPos0 = coordinator->GetComponent<Position>(controlPoints[0]);
    auto const& cpPos1 = coordinator->GetComponent<Position>(controlPoints[1]);

    result[0] = (cpPos1.vec - cpPos0.vec - cubicPowerCoeff[0] * t*t*t) / t;

    for (int i=1; i < result.size(); ++i) {
        float t = chordsLen[i-1];
        result[i] = result[i-1] + 2.f*squarePowerCoeff[i-1]*t + 3.f*cubicPowerCoeff[i-1]*t*t;
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurveSystem::ConstantCoeffInPowerBasis(const std::vector<alg::Vec3> &linearPowerCoeff, const std::vector<alg::Vec3> &squarePowerCoeff, const std::vector<alg::Vec3> cubicPowerCoeff, const std::vector<float> &chordsLen, const ControlPoints &cps) const
{
    auto const& controlPoints = cps.GetPoints();

    std::vector<alg::Vec3> result(squarePowerCoeff.size() - 1);

    result[0] = coordinator->GetComponent<Position>(controlPoints[0]).vec;

    for (int i=1; i < result.size(); ++i) {
        float t = chordsLen[i-1];
        result[i] = result[i-1] + linearPowerCoeff[i-1]*t + squarePowerCoeff[i-1]*t*t + cubicPowerCoeff[i-1]*t*t*t;
    }

    return result;
}


std::vector<float> InterpolationCurveSystem::ChordLengthsBetweenControlPoints(const ControlPoints &cps) const
{
    auto const& controlPoints = cps.GetPoints();

    std::vector<float> result;

    if (controlPoints.size() <= 1)
        return result;

    result.reserve(controlPoints.size() - 1);

    for (int i=1; i < controlPoints.size(); ++i) {
        auto const& p0 = coordinator->GetComponent<Position>(controlPoints[i-1]);
        auto const& p1 = coordinator->GetComponent<Position>(controlPoints[i]);

        result.push_back(alg::Distance(p0.vec, p1.vec));
    }

    return result;
}


std::vector<float> InterpolationCurveSystem::SubdiagonalElems(const std::vector<float> &chordLengths) const
{
    std::vector<float> result(chordLengths.size() - 2);

    for (int i=0; i < result.size(); ++i) {
        result[i] = chordLengths[i+1] / (chordLengths[i+1] + chordLengths[i+2]);
    }

    return result;
}


std::vector<float> InterpolationCurveSystem::DiagonalElems(const std::vector<float> &chordLengths) const
{
    return std::vector<float>(chordLengths.size() - 1, 2.f);
}


std::vector<float> InterpolationCurveSystem::SuperdiagonalElems(const std::vector<float> &chordLengths) const
{
    std::vector<float> result(chordLengths.size() - 2);

    for (int i=0; i < result.size(); ++i) {
        result[i] = chordLengths[i+1] / (chordLengths[i] + chordLengths[i+1]);
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurveSystem::EquationResultsElems(const std::vector<float> &chordLengths, const ControlPoints &cps) const
{
    auto const& controlPoints = cps.GetPoints();
    std::vector<alg::Vec3> result(chordLengths.size() - 1);

    for (int i=0; i < result.size(); ++i) {
        auto const& posI = coordinator->GetComponent<Position>(controlPoints[i]);
        auto const& posIPlus1 = coordinator->GetComponent<Position>(controlPoints[i+1]);
        auto const& posIPlus2 = coordinator->GetComponent<Position>(controlPoints[i+2]);

        result[i] = 3.f * ((posIPlus2.vec - posIPlus1.vec) / chordLengths[i+1] - (posIPlus1.vec - posI.vec) / chordLengths[i]) / (chordLengths[i+1] + chordLengths[i]);
    }

    return result;
}



