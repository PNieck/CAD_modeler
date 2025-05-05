#include <CAD_modeler/model/systems/interpolationCurvesRenderingSystem.hpp>

#include <ecs/coordinator.hpp>

#include <CAD_modeler/model/systems/toUpdateSystem.hpp>
#include <CAD_modeler/model/systems/selectionSystem.hpp>
#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <algebra/systemsOfLinearEquasions.hpp>
#include <algebra/cubicPolynomials.hpp>

#include <cassert>


void InterpolationCurvesRenderingSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<InterpolationCurvesRenderingSystem>();
}


Entity InterpolationCurvesRenderingSystem::AddCurve(const std::vector<Entity> &cps)
{
    const Entity curve = coordinator->GetSystem<CurveControlPointsSystem>()->CreateControlPoints(
        cps, Coordinator::GetSystemID<InterpolationCurvesRenderingSystem>()
    );

    coordinator->AddComponent<Mesh>(curve, Mesh());

    UpdateMesh(
        curve,
        coordinator->GetComponent<CurveControlPoints>(curve)
    );

    entities.insert(curve);

    return curve;
}


void InterpolationCurvesRenderingSystem::Render(const alg::Mat4x4& cameraMtx) const
{
    if (entities.empty()) {
        return;
    }

    auto const& selectionSystem = coordinator->GetSystem<SelectionSystem>();
    auto const& shader = ShaderRepository::GetInstance().GetBezierCurveShader();

    shader.Use();
    shader.SetColor(alg::Vec4(1.0f));
    shader.SetMVP(cameraMtx);

    for (auto const entity: entities) {
        const bool selection = selectionSystem->IsSelected(entity);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));

        auto const& mesh = coordinator->GetComponent<Mesh>(entity);
        mesh.Use();

        glPatchParameteri(GL_PATCH_VERTICES, 4);
	    glDrawElements(GL_PATCHES, mesh.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

        if (selection)
            shader.SetColor(alg::Vec4(1.0f));
    }
}


void InterpolationCurvesRenderingSystem::Update() const
{
    auto const& toUpdateSystem = coordinator->GetSystem<ToUpdateSystem>();

    for (const auto entity: toUpdateSystem->GetEntitiesToUpdate<InterpolationCurvesRenderingSystem>()) {
        auto const cps = coordinator->GetComponent<CurveControlPoints>(entity);

        if (!cps.Empty())
            UpdateMesh(entity, cps);
        else
            coordinator->DestroyEntity(entity);
    }

    toUpdateSystem->UnmarkAll<InterpolationCurvesRenderingSystem>();
}


void InterpolationCurvesRenderingSystem::UpdateMesh(const Entity entity, const CurveControlPoints& cps) const
{
    coordinator->EditComponent<Mesh>(entity,
        [&cps, this](Mesh& mesh) {
            mesh.Update(
                GenerateMeshVertices(cps),
                GenerateMeshIndices(cps)
            );
        }
    );
}


std::vector<float> InterpolationCurvesRenderingSystem::GenerateMeshVertices(const CurveControlPoints &cps) const
{
    std::vector<float> result;

    if (cps.Size() < 2)
        return result;

    auto powerBasis = FindInterpolationsPolynomialsInPowerBasis(cps);
    result.reserve(powerBasis.size() * 4 * 3);

    for (auto const& polynomial: powerBasis) {
        auto bernsteinBasis = FromPowerToBernsteinBasis(polynomial);

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


std::vector<uint32_t> InterpolationCurvesRenderingSystem::GenerateMeshIndices(const CurveControlPoints &cps) const
{
    std::vector<uint32_t> result((cps.Size() - 1) * 4);

    for (size_t i=0; i < result.size(); ++i) {
        result[i] = static_cast<uint32_t>(i);
    }

    return result;
}


std::vector<alg::Vector4<alg::Vec3>> InterpolationCurvesRenderingSystem::FindInterpolationsPolynomialsInPowerBasis(const CurveControlPoints& cps) const
{
    const auto ctrlPts = PreprocessControlPoints(cps);

    const auto chordLengths = ChordLengthsBetweenControlPoints(ctrlPts);

    const auto squareCoeff = SquareCoefInPowerBasis(ctrlPts, chordLengths);
    const auto cubicCoeff = CubicCoefInPowerBasis(squareCoeff, chordLengths);
    const auto linearCoeff = LinearCoefInPowerBasis(squareCoeff, cubicCoeff, chordLengths, ctrlPts);
    const auto constCoeff = ConstantCoefInPowerBasis(linearCoeff, squareCoeff, cubicCoeff, chordLengths, ctrlPts);

    std::vector<alg::Vector4<alg::Vec3>> result(constCoeff.size());

    for (size_t i=0; i < constCoeff.size(); ++i) {
        result[i] = alg::Vector4(
            constCoeff[i],
            linearCoeff[i],
            squareCoeff[i],
            cubicCoeff[i]
        );
    }

    for (size_t i=0; i < result.size(); ++i) {
        ChangePolynomialInPowerBaseDomainFrom0To1(result[i], chordLengths[i]);
    }

    return result;
}


void InterpolationCurvesRenderingSystem::ChangePolynomialInPowerBaseDomainFrom0To1(alg::Vector4<alg::Vec3> &polynomial, float domain) const
{
    polynomial.Y() *= domain;
    polynomial.Z() *= domain*domain;
    polynomial.W() *= domain*domain*domain;
}


std::vector<Entity> InterpolationCurvesRenderingSystem::PreprocessControlPoints(const CurveControlPoints &cps) const
{
    std::vector<Entity> result;
    result.reserve(cps.Size());

    // Add first control point
    auto it = cps.GetPoints().begin();
    result.push_back(*it);

    auto prevPos = coordinator->GetComponent<Position>(*it);

    while (++it != cps.GetPoints().end()) {
        auto actPos = coordinator->GetComponent<Position>(*it);

        if (prevPos.vec != actPos.vec)
            result.push_back(*it);

        prevPos = actPos;
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurvesRenderingSystem::SquareCoefInPowerBasis(const std::vector<Entity>& ctrlPts, const std::vector<float>& chordsLen) const
{
    if (ctrlPts.size() == 2) {
        return { alg::Vec3(0), alg::Vec3(0) };
    }

    auto superDiagonal = SuperdiagonalElems(chordsLen);
    auto diagonal = DiagonalElems(chordsLen);
    auto subdiagonal = SubdiagonalElems(chordsLen);
    auto equationResults = EquationResultsElems(chordsLen, ctrlPts);

    auto coeff = alg::SolveSystemOfLinearEquationsWithTridiagonalMtx(
        superDiagonal,
        diagonal,
        subdiagonal,
        equationResults
    );

    assert(coeff.has_value());

    std::vector<alg::Vec3> result(coeff.value().size() + 2);

    result[0] = alg::Vec3(0);

    for (size_t i=1; i <= result.size() - 2; ++i) {
        result[i] = coeff.value()[i-1];
    }

    result[result.size() - 1] = alg::Vec3(0);

    return result;
}


std::vector<alg::Vec3> InterpolationCurvesRenderingSystem::CubicCoefInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoef, const std::vector<float>& chordsLen) const
{
    std::vector<alg::Vec3> result(squarePowerCoef.size() - 1);

    for (size_t i=0; i < result.size(); ++i) {
        result[i] = (squarePowerCoef[i+1] - squarePowerCoef[i]) / (3.f * chordsLen[i]);
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurvesRenderingSystem::LinearCoefInPowerBasis(
    const std::vector<alg::Vec3>& squarePowerCoef,
    const std::vector<alg::Vec3>& cubicPowerCoef,
    const std::vector<float>& chordsLen,
    const std::vector<Entity>& ctrlPts) const
{
    std::vector<alg::Vec3> result(squarePowerCoef.size() - 1);

    float t = chordsLen[0];

    auto const& cpPos0 = coordinator->GetComponent<Position>(ctrlPts[0]);
    auto const& cpPos1 = coordinator->GetComponent<Position>(ctrlPts[1]);

    result[0] = (cpPos1.vec - cpPos0.vec - cubicPowerCoef[0] * t*t*t) / t;

    for (size_t i=1; i < result.size(); ++i) {
        t = chordsLen[i-1];
        result[i] = result[i-1] + 2.f*squarePowerCoef[i-1]*t + 3.f*cubicPowerCoef[i-1]*t*t;
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurvesRenderingSystem::ConstantCoefInPowerBasis(
    const std::vector<alg::Vec3>& linearPowerCoef,
    const std::vector<alg::Vec3>& squarePowerCoef,
    const std::vector<alg::Vec3>& cubicPowerCoef,
    const std::vector<float>& chordsLen,
    const std::vector<Entity>& ctrlPts) const
{
    std::vector<alg::Vec3> result(squarePowerCoef.size() - 1);

    result[0] = coordinator->GetComponent<Position>(ctrlPts[0]).vec;

    for (size_t i=1; i < result.size(); ++i) {
        const float t = chordsLen[i-1];
        result[i] = result[i-1] + linearPowerCoef[i-1]*t + squarePowerCoef[i-1]*t*t + cubicPowerCoef[i-1]*t*t*t;
    }

    return result;
}


std::vector<float> InterpolationCurvesRenderingSystem::ChordLengthsBetweenControlPoints(const std::vector<Entity> &ctrlPts) const
{
    std::vector<float> result;

    if (ctrlPts.size() <= 1)
        return result;

    result.reserve(ctrlPts.size() - 1);

    for (size_t i=1; i < ctrlPts.size(); ++i) {
        auto const& p0 = coordinator->GetComponent<Position>(ctrlPts[i-1]);
        auto const& p1 = coordinator->GetComponent<Position>(ctrlPts[i]);

        result.push_back(alg::Distance(p0.vec, p1.vec));
    }

    return result;
}


std::vector<float> InterpolationCurvesRenderingSystem::SubdiagonalElems(const std::vector<float> &chordLengths) const
{
    std::vector<float> result(chordLengths.size() - 2);

    for (size_t i=0; i < result.size(); ++i) {
        result[i] = chordLengths[i+1] / (chordLengths[i+1] + chordLengths[i+2]);
    }

    return result;
}


std::vector<float> InterpolationCurvesRenderingSystem::DiagonalElems(const std::vector<float> &chordLengths) const
{
    return std::vector(chordLengths.size() - 1, 2.f);
}


std::vector<float> InterpolationCurvesRenderingSystem::SuperdiagonalElems(const std::vector<float> &chordLengths) const
{
    std::vector<float> result(chordLengths.size() - 2);

    for (size_t i=0; i < result.size(); ++i) {
        result[i] = chordLengths[i+1] / (chordLengths[i] + chordLengths[i+1]);
    }

    return result;
}


std::vector<alg::Vec3> InterpolationCurvesRenderingSystem::EquationResultsElems(const std::vector<float> &chordLengths, const std::vector<Entity>& ctrlPts) const
{
    std::vector<alg::Vec3> result(chordLengths.size() - 1);

    for (size_t i=0; i < result.size(); ++i) {
        auto const& posI = coordinator->GetComponent<Position>(ctrlPts[i]);
        auto const& posIPlus1 = coordinator->GetComponent<Position>(ctrlPts[i+1]);
        auto const& posIPlus2 = coordinator->GetComponent<Position>(ctrlPts[i+2]);

        result[i] = 3.f * ((posIPlus2.vec - posIPlus1.vec) / chordLengths[i+1] - (posIPlus1.vec - posI.vec) / chordLengths[i]) / (chordLengths[i+1] + chordLengths[i]);
    }

    return result;
}
