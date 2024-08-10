#pragma once

#include <ecs/system.hpp>

#include <algebra/vec4.hpp>

#include "utils/nameGenerator.hpp"
#include "curveControlPointsSystem.hpp"
#include "shaders/shaderRepository.hpp"

#include <vector>


class InterpolationCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    inline void Init(ShaderRepository* shadersRepo)
        { this->shaderRepo = shadersRepo; }

    Entity CreateCurve(const std::vector<Entity>& entities);

    inline void AddControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity); }

    inline void DeleteControlPoint(Entity bezierCurve, Entity entity)
        { coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity); }

    void Render(const alg::Mat4x4& cameraMtx) const;

private:
    ShaderRepository* shaderRepo;
    NameGenerator nameGenerator;

    void UpdateEntities() const;

    void UpdateMesh(Entity entity, const CurveControlPoints& cps) const;

    std::vector<float> GenerateMeshVertices(const CurveControlPoints& cps) const;
    std::vector<uint32_t> GenerateMeshIndices(const CurveControlPoints& cps) const;

    using Polynomial = alg::Vec4;

    std::vector<alg::Vector4<alg::Vec3>> FindInterpolationsPolynomialsInPowerBasis(const CurveControlPoints& cps) const;

    void ChangePolynomialInPowerBaseDomainFrom0To1(alg::Vector4<alg::Vec3>& polynomial, float domain) const;

    /// @brief Function removes neighboring control points with the same positions
    std::vector<Entity> PreprocessControlPoints(const CurveControlPoints& cps) const;

    std::vector<alg::Vec3> ConstantCoeffInPowerBasis(const std::vector<alg::Vec3>& linearPowerCoeff, const std::vector<alg::Vec3>& squarePowerCoeff, const std::vector<alg::Vec3>& cubicPowerCoeff, const std::vector<float>& chordsLen, const std::vector<Entity>& ctrlPts) const;
    std::vector<alg::Vec3> LinearCoeffInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoeff, const std::vector<alg::Vec3>& cubicPowerCoeff, const std::vector<float>& chordsLen, const std::vector<Entity>& ctrlPts) const;
    std::vector<alg::Vec3> SquareCoeffInPowerBasis(const std::vector<Entity>& ctrlPts, const std::vector<float>& chordsLen) const;
    std::vector<alg::Vec3> CubicCoeffInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoeff, const std::vector<float>& chordsLen) const;

    std::vector<float> ChordLengthsBetweenControlPoints(const std::vector<Entity>& cps) const;
    std::vector<float> SubdiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<float> DiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<float> SuperdiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<alg::Vec3> EquationResultsElems(const std::vector<float>& chordLengths, const std::vector<Entity>& ctrlPts) const;

    

};
 