#include <CAD_modeler/model/systems/intersectionsSystem.hpp>

#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>
#include <CAD_modeler/model/systems/c2CylinderSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/vectorSystem.hpp>

#include <CAD_modeler/model/systems/intersectionSystem/torusSurface.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/c0Surface.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/c2Surface.hpp>

#include <ecs/coordinator.hpp>

#include <optimization/conjugateGradientMethod.hpp>
#include <optimization/lineSearchMethods/dichotomyLineSearch.hpp>
#include <optimization/stopConditions/smallGradient.hpp>

#include <rootFinding/newtonMethod.hpp>

#include <cassert>

// TODO: remove
#include <iostream>


using namespace interSys;


void IntersectionSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<IntersectionSystem>();
}


bool IntersectionSystem::CanBeIntersected(const Entity entity) const
{
    if (coordinator->GetSystem<C0PatchesSystem>()->GetEntities().contains(entity))
        return true;

    if (coordinator->GetSystem<C2SurfaceSystem>()->GetEntities().contains(entity))
        return true;

    if (coordinator->GetSystem<C2CylinderSystem>()->GetEntities().contains(entity))
        return true;

    if (coordinator->GetSystem<ToriSystem>()->GetEntities().contains(entity))
        return true;

    return false;
}


void IntersectionSystem::FindIntersection(const Entity e1, const Entity e2, const float step)
{
    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));
    assert(e1 != e2);

    const auto surface1 = GetSurface(e1);
    const auto surface2 = GetSurface(e2);

    const auto initSol = FindFirstApproximation(*surface1, *surface2);
    auto firstPointOpt = FindFirstIntersectionPoint(*surface1, *surface2, initSol);

    if (!firstPointOpt.has_value()) {
        std::cout << "Cannot find first point\n";
        return;
    }

    FindIntersection(*surface1, *surface2, firstPointOpt.value(), step);
}


void IntersectionSystem::FindIntersection(const Entity e1, const Entity e2, const float step, const Position &guidance)
{
    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));
    assert(e1 != e2);

    const auto surface1 = GetSurface(e1);
    const auto surface2 = GetSurface(e2);

    const auto nearestPoint1 = NearestPoint(*surface1, guidance);
    const auto nearestPoint2 = NearestPoint(*surface2, guidance);
    if (!nearestPoint1.has_value() || !nearestPoint2.has_value()) {
        std::cout << "Cannot find nearest point\n";
        return;
    }

    auto [u1, v1] = nearestPoint1.value();
    auto [u2, v2] = nearestPoint2.value();

    const IntersectionPoint startingApprox(u1, v1, u2, v2);

    const auto firstPointOpt = FindFirstIntersectionPoint(*surface1, *surface2, startingApprox);
    if (!firstPointOpt.has_value()) {
        std::cout << "Cannot find first point\n";
        return;
    }

    FindIntersection(*surface1, *surface2, firstPointOpt.value(), step);
}


std::unique_ptr<Surface> IntersectionSystem::GetSurface(const Entity entity) const
{
    if (coordinator->GetSystem<ToriSystem>()->GetEntities().contains(entity))
        return std::make_unique<TorusSurface>(*coordinator, entity);

    if (coordinator->GetSystem<C0PatchesSystem>()->GetEntities().contains(entity))
        return std::make_unique<C0Surface>(*coordinator, entity);

    if (coordinator->GetSystem<C2SurfaceSystem>()->GetEntities().contains(entity))
        return std::make_unique<C2Surface>(*coordinator, coordinator->GetComponent<C2Patches>(entity));

    if (coordinator->GetSystem<C2CylinderSystem>()->GetEntities().contains(entity))
        return std::make_unique<C2Surface>(*coordinator, coordinator->GetComponent<C2CylinderPatches>(entity));

    throw std::runtime_error("Entity cannot be used to calculate intersection curve");
}


IntersectionPoint IntersectionSystem::FindFirstApproximation(Surface& s1, Surface& s2) const
{
    constexpr int sampleCntInOneDim = 15;

    const float maxU1 = s1.MaxUInitSampleVal();
    const float minU1 = s1.MinUInitSampleVal();

    const float maxV1 = s1.MaxVInitSampleVal();
    const float minV1 = s1.MinVInitSampleVal();

    const float maxU2 = s2.MaxUInitSampleVal();
    const float minU2 = s2.MinUInitSampleVal();

    const float maxV2 = s2.MaxVInitSampleVal();
    const float minV2 = s2.MinVInitSampleVal();

    const float deltaU1 = (maxU1 - minU1) / static_cast<float>(sampleCntInOneDim + 1);
    const float deltaV1 = (maxV1 - minV1) / static_cast<float>(sampleCntInOneDim + 1);
    const float deltaU2 = (maxU2 - minU2) / static_cast<float>(sampleCntInOneDim + 1);
    const float deltaV2 = (maxV2 - minV2) / static_cast<float>(sampleCntInOneDim + 1);

    float minDist = std::numeric_limits<float>::infinity();
    IntersectionPoint result;

    for (int i = 1; i <= sampleCntInOneDim; ++i) {
        const float u1 = deltaU1 * static_cast<float>(i) + minU1;

        for (int j = 1; j <= sampleCntInOneDim; ++j) {
            const float v1 = deltaV1 * static_cast<float>(j) + minV1;

            for (int k = 1; k <= sampleCntInOneDim; ++k) {
                const float u2 = deltaU2 * static_cast<float>(k) + minU2;

                for (int l = 1; l <= sampleCntInOneDim; ++l) {
                    const float v2 = deltaV2 * static_cast<float>(l) + minV2;

                    auto point1 = s1.PointOnSurface(u1, v1);
                    auto point2 = s2.PointOnSurface(u2, v2);

                    const float dist = DistanceSquared(point1, point2);
                    if (minDist > dist) {
                        minDist = dist;

                        result.V1() = v1;
                        result.U1() = u1;
                        result.V2() = v2;
                        result.U2() = u2;
                    }
                }
            }
        }
    }

    return result;
}


class DistanceBetweenPoints final : public opt::FunctionToOptimize {
public:
    explicit DistanceBetweenPoints(Surface& s1, Surface& s2):
        surface1(s1), surface2(s2) {}

    float Value(const std::vector<float> &args) override {
        const auto point1 = surface1.PointOnSurface(args[0], args[1]);
        const auto point2 = surface2.PointOnSurface(args[2], args[3]);

        return DistanceSquared(point1, point2);
    }


    std::vector<float> Gradient(const std::vector<float> &args) override {
        const auto point1 = surface1.PointOnSurface(args[0], args[1]);
        const auto point2 = surface2.PointOnSurface(args[2], args[3]);

        const auto partDivU1 = surface1.PartialDerivativeU(args[0], args[1]);
        const auto partDivU2 = surface2.PartialDerivativeU(args[2], args[3]);

        const auto partDivV1 = surface1.PartialDerivativeV(args[0], args[1]);
        const auto partDivV2 = surface2.PartialDerivativeV(args[2], args[3]);

        const auto diff = point1 - point2;

        return {
            2.f * Dot(diff, partDivU1),     // df/du1
            2.f * Dot(diff, partDivV1),     // df/dv1
            -2.f * Dot(diff, partDivU2),    // df/du2
            -2.f * Dot(diff, partDivV2)     // df/dv2
        };
    }

private:
    Surface& surface1;
    Surface& surface2;
};


class NearZeroCondition final : public opt::StopCondition {
public:
    explicit NearZeroCondition(const float eps=1e-7): eps(eps) {}

    bool ShouldStop(opt::FunctionToOptimize &fun, const std::vector<float> &args) override {
        return fun.Value(args) <= eps;
    }

private:
    float eps;
};


std::optional<IntersectionPoint> IntersectionSystem::FindFirstIntersectionPoint(Surface& s1, Surface& s2, const IntersectionPoint& initSol) const
{
    const std::vector startingPoint = {
        initSol.U1(),
        initSol.V1(),
        initSol.U2(),
        initSol.V2()
    };

    opt::DichotomyLineSearch lineSearch(0, 0.1f, 1e-7f);
    NearZeroCondition stopCond;
    DistanceBetweenPoints fun(s1, s2);

    const auto sol = ConjugateGradientMethod(fun, lineSearch, startingPoint, 100, stopCond);

    if (!sol.has_value())
        return std::nullopt;

    IntersectionPoint result(
        sol.value()[0],
        sol.value()[1],
        sol.value()[2],
        sol.value()[3]
    );

    if (!SolutionInDomains(result, s1, s2))
        return std::nullopt;

    if (ErrorRate(s1, s2, result) > 1e-5)
        return std::nullopt;

    return result;
}


class NextPointDistFun final : public root::FunctionToFindRoot {
public:
    NextPointDistFun(Surface& s1, Surface& s2, const alg::Vec3& prevPoint,
                     const alg::Vec3& tangent, const float step):
        surface1(s1), surface2(s2), prevPoint(prevPoint), tangent(tangent), step(step) {}

    alg::Vec4 Value(alg::Vec4 args) override {
        if (!IntersectionSystem::SolutionInDomains(IntersectionPoint(args), surface1, surface2)) {
            outsideDomain = true;
            return alg::Vec4(NAN);
        }

        const auto point1 = surface1.PointOnSurface(args.X(), args.Y());
        const auto point2 = surface2.PointOnSurface(args.Z(), args.W());

        auto diff = point1 - point2;
        auto v = Dot(point1 - prevPoint, tangent) - step;

        return {
            diff.X(),
            diff.Y(),
            diff.Z(),
            v
        };
    }

    alg::Mat4x4 Jacobian(alg::Vec4 args) override {
        const auto derE1X = surface1.PartialDerivativeU(args.X(), args.Y());
        const auto derE1Y = surface1.PartialDerivativeV(args.X(), args.Y());

        const auto derE2Z = surface2.PartialDerivativeU(args.Z(), args.W());
        const auto derE2W = surface2.PartialDerivativeV(args.Z(), args.W());

        const float partX = Dot(tangent, derE1X);
        const float partY = Dot(tangent, derE1Y);

        return {
            derE1X.X(), derE1Y.X(), -derE2Z.X(), -derE2W.X(),
            derE1X.Y(), derE1Y.Y(), -derE2Z.Y(), -derE2W.Y(),
            derE1X.Z(), derE1Y.Z(), -derE2Z.Z(), -derE2W.Z(),
                 partX,      partY,         0.f,         0.f
        };
    }

    [[nodiscard]]
    bool WasEvaluatedOutsideTheDomain() const
        { return outsideDomain; }

private:
    Surface& surface1;
    Surface& surface2;

    alg::Vec3 prevPoint;
    alg::Vec3 tangent;
    float step;

    bool outsideDomain = false;
};


std::tuple<std::optional<IntersectionPoint>, bool> IntersectionSystem::FindNextIntersectionPoint(Surface& s1, Surface& s2, const IntersectionPoint &prevSol, float step) const
{
    float remainingDist = step;
    const float minStep = step / 1024.f;
    std::optional<alg::Vec4> nextPoint;
    bool endEncountered = false;

    const auto vectorSys = coordinator->GetSystem<VectorSystem>();

    do {
        alg::Vec3 normal1 = s1.NormalVector(prevSol.U1(), prevSol.V1());
        alg::Vec3 normal2 = s2.NormalVector(prevSol.U2(), prevSol.V2());

        alg::Vec3 tangent = Cross(normal1, normal2).Normalize();
        alg::Vec3 prevPoint = s1.PointOnSurface(prevSol.U1(), prevSol.V1());

        vectorSys->AddVector(tangent, prevPoint);
        vectorSys->AddVector(normal1, prevPoint);
        vectorSys->AddVector(normal2, prevPoint);

        NextPointDistFun fun(
            s1, s2,
            prevPoint,
            tangent,
            step
        );

        nextPoint = NewtonMethod(fun, prevSol.AsVector(), 1e-5);
        if (!nextPoint.has_value()) {
            step /= 2.f;
            if (step < minStep)
                return {std::nullopt, endEncountered};
        }
        else if (fun.WasEvaluatedOutsideTheDomain()) {
            endEncountered = true;
            step /= 2.f;
            remainingDist /= 2.f;
        }
        else {
            remainingDist -= step;
            step = remainingDist;
        }

    } while (remainingDist > 0.f);

    return { IntersectionPoint(nextPoint.value()), endEncountered };
}


class NearestPointFun final : public opt::FunctionToOptimize {
public:
    explicit NearestPointFun(Surface& s, const Position& guidance):
        surface(s), guidance(guidance) {}

    float Value(const std::vector<float> &args) override {
        const auto point = surface.PointOnSurface(args.at(0), args.at(1));

        return DistanceSquared(point, guidance.vec);
    }


    std::vector<float> Gradient(const std::vector<float> &args) override {
        const auto point = surface.PointOnSurface(args.at(0), args.at(1));

        const auto partDivU = surface.PartialDerivativeU(args.at(0), args.at(1));
        const auto partDivV = surface.PartialDerivativeV(args.at(0), args.at(1));

        const auto diff = point - guidance.vec;

        return {
            2.f * Dot(diff, partDivU),
            2.f * Dot(diff, partDivV)
        };
    }

private:
    Surface& surface;
    Position guidance;
};


std::optional<std::tuple<float, float>> IntersectionSystem::NearestPoint(Surface &s, const Position &guidance) const
{
    auto [uApprox, vApprox] = NearestPointApproximation(s, guidance);

    const std::vector startingPoint {
        uApprox, vApprox
    };

    opt::DichotomyLineSearch lineSearch(0, 0.1f, 1e-7f);
    opt::SmallGradient stopCond;
    NearestPointFun fun(s, guidance);

    const auto solOpt = ConjugateGradientMethod(fun, lineSearch, startingPoint, 100, stopCond);
    if (!solOpt.has_value())
        return std::nullopt;

    const auto& sol = solOpt.value();

    if (!SolutionInDomain(s, sol.at(0), sol.at(1)))
        return std::nullopt;

    return std::make_tuple(sol.at(0), sol.at(1));
}


std::tuple<float, float> IntersectionSystem::NearestPointApproximation(Surface &s, const Position &guidance) const
{
    constexpr int sampleCntInOneDim = 30;

    const float maxU = s.MaxUInitSampleVal();
    const float minU = s.MinUInitSampleVal();

    const float maxV = s.MaxVInitSampleVal();
    const float minV = s.MinVInitSampleVal();

    const float deltaU = (maxU - minU) / static_cast<float>(sampleCntInOneDim + 1);
    const float deltaV = (maxV - minV) / static_cast<float>(sampleCntInOneDim + 1);

    float minDist = std::numeric_limits<float>::infinity();
    float resultU, resultV;

    for (int i = 1; i <= sampleCntInOneDim; ++i) {
        const float u = deltaU * static_cast<float>(i) + minU;

        for (int j = 1; j <= sampleCntInOneDim; ++j) {
            const float v = deltaV * static_cast<float>(j) + minV;

            auto point = s.PointOnSurface(u, v);

            const float dist = DistanceSquared(point, guidance.vec);
            if (minDist > dist) {
                minDist = dist;

                resultU = u;
                resultV = v;
            }
        }
    }

    return {resultU, resultV};
}


void IntersectionSystem::FindIntersection(Surface &s1, Surface &s2, const IntersectionPoint &initPoint, const float step)
{
    const auto pointSys = coordinator->GetSystem<PointsSystem>();

    const auto firstPoint = s1.PointOnSurface(initPoint.U1(), initPoint.V1());
    pointSys->CreatePoint(firstPoint);

    auto [solOpt, lastPt] = FindNextIntersectionPoint(s1, s2, initPoint, step);
    if (!solOpt.has_value()) {
        std::cout << "Cannot find second point\n";
        return;
    }

    auto& sol = solOpt.value();
    pointSys->CreatePoint(s1.PointOnSurface(sol.U1(), sol.V1()));
    if (lastPt) {
        FindOpenIntersection(initPoint, s1, s2, step);
        return;
    }

    Position newPoint;
    do {
        std::tie(solOpt, lastPt) = FindNextIntersectionPoint(s1, s2, sol, step);
        if (!solOpt.has_value()) {
            std::cout << "Cannot find next point\n";
            return;
        }

        sol = solOpt.value();
        pointSys->CreatePoint(s1.PointOnSurface(sol.U1(), sol.V1()));

        newPoint = s1.PointOnSurface(sol.U1(), sol.V1());

        if (lastPt) {
            FindOpenIntersection(initPoint, s1, s2, step);
            return;
        }
    } while (DistanceSquared(firstPoint, newPoint.vec) > step*step);
}


void IntersectionSystem::FindOpenIntersection(const IntersectionPoint& firstPoint, Surface& s1, Surface& s2, const float step) const
{
    const auto pointSys = coordinator->GetSystem<PointsSystem>();

    IntersectionPoint prevSol(firstPoint.U2(), firstPoint.V2(), firstPoint.U1(), firstPoint.V1());
    bool lastPt;
    std::optional<IntersectionPoint> solOpt;

    do {
        // Passing solutions in reversed order to traverse intersection in other direction
        std::tie(solOpt, lastPt) = FindNextIntersectionPoint(s2, s1, prevSol, step);

        if (!solOpt.has_value()) {
            std::cout << "Cannot find first point\n";
            return;
        }

        auto& sol = solOpt.value();

        const auto point = s1.PointOnSurface(sol.U2(), sol.V2());
        pointSys->CreatePoint(point);

        prevSol = sol;
    } while (!lastPt);
}


bool IntersectionSystem::SolutionInDomains(const IntersectionPoint &sol, Surface &s1, Surface &s2) {
    return SolutionInDomain(s1, sol.U1(), sol.V1()) && SolutionInDomain(s2, sol.U2(), sol.V2());
}


bool IntersectionSystem::SolutionInDomain(Surface &s, const float u, const float v) {
    return s.MinU() <= u && s.MaxU() >= u && s.MinV() <= v && s.MaxV() >= v;
}


float IntersectionSystem::ErrorRate(Surface& s1, Surface& s2, const IntersectionPoint &intPt) const
{
    const auto point1 = s1.PointOnSurface(intPt.U1(), intPt.V1());
    const auto point2 = s2.PointOnSurface(intPt.U2(), intPt.V2());

    return DistanceSquared(point1, point2);
}
