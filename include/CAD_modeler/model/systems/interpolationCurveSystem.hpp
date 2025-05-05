#pragma once

#include <ecs/system.hpp>

#include <algebra/vec4.hpp>

#include "curveControlPointsSystem.hpp"

#include <vector>


class InterpolationCurveSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    Entity CreateCurve(const std::vector<Entity>& entities);

    void AddControlPoint(const Entity bezierCurve, const Entity entity) const {
        coordinator->GetSystem<CurveControlPointsSystem>()->AddControlPoint(bezierCurve, entity, Coordinator::GetSystemID<InterpolationCurveSystem>());
    }

    void DeleteControlPoint(const Entity bezierCurve, const Entity entity) const {
        coordinator->GetSystem<CurveControlPointsSystem>()->DeleteControlPoint(bezierCurve, entity, Coordinator::GetSystemID<InterpolationCurveSystem>());
    }

    void MergeControlPoints(const Entity curve, const Entity oldCP, const Entity newCP) const {
        coordinator->GetSystem<CurveControlPointsSystem>()->MergeControlPoints(curve, oldCP, newCP, Coordinator::GetSystemID<InterpolationCurveSystem>());
    }

    void Render(const alg::Mat4x4& cameraMtx) const;

    void Update() const;

private:
    void UpdateMesh(Entity entity, const CurveControlPoints& cps) const;

    std::vector<float> GenerateMeshVertices(const CurveControlPoints& cps) const;
    std::vector<uint32_t> GenerateMeshIndices(const CurveControlPoints& cps) const;

    using Polynomial = alg::Vec4;

    std::vector<alg::Vector4<alg::Vec3>> FindInterpolationsPolynomialsInPowerBasis(const CurveControlPoints& cps) const;

    void ChangePolynomialInPowerBaseDomainFrom0To1(alg::Vector4<alg::Vec3>& polynomial, float domain) const;

    /// @brief Function removes neighboring control points with the same positions
    std::vector<Entity> PreprocessControlPoints(const CurveControlPoints& cps) const;

    std::vector<alg::Vec3> ConstantCoefInPowerBasis(const std::vector<alg::Vec3>& linearPowerCoef, const std::vector<alg::Vec3>& squarePowerCoef, const std::vector<alg::Vec3>& cubicPowerCoef, const std::vector<float>& chordsLen, const std::vector<Entity>& ctrlPts) const;
    std::vector<alg::Vec3> LinearCoefInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoef, const std::vector<alg::Vec3>& cubicPowerCoef, const std::vector<float>& chordsLen, const std::vector<Entity>& ctrlPts) const;
    std::vector<alg::Vec3> SquareCoefInPowerBasis(const std::vector<Entity>& ctrlPts, const std::vector<float>& chordsLen) const;
    std::vector<alg::Vec3> CubicCoefInPowerBasis(const std::vector<alg::Vec3>& squarePowerCoef, const std::vector<float>& chordsLen) const;

    std::vector<float> ChordLengthsBetweenControlPoints(const std::vector<Entity>& cps) const;
    std::vector<float> SubdiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<float> DiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<float> SuperdiagonalElems(const std::vector<float>& chordLengths) const;
    std::vector<alg::Vec3> EquationResultsElems(const std::vector<float>& chordLengths, const std::vector<Entity>& ctrlPts) const;
};
 