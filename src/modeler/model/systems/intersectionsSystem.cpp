#include <CAD_modeler/model/systems/intersectionsSystem.hpp>

#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>
#include <CAD_modeler/model/systems/c2CylinderSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/interpolationCurvesRenderingSystem.hpp>

#include <CAD_modeler/model/systems/intersectionSystem/torusSurface.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/c0Surface.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/c2Surface.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/nextPointFinder.hpp>
#include <CAD_modeler/model/systems/intersectionSystem/domainChecks.hpp>

#include <ecs/coordinator.hpp>

#include <optimization/conjugateGradientMethod.hpp>
#include <optimization/lineSearchMethods/dichotomyLineSearch.hpp>
#include <optimization/stopConditions/smallGradient.hpp>

#include <algebra/vec2.hpp>

#include <cassert>

// TODO: remove
#include <iostream>


using namespace interSys;


void IntersectionSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<IntersectionSystem>();

    coordinator.RegisterComponent<IntersectionCurve>();
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


std::optional<Entity> IntersectionSystem::FindIntersection(const Entity e1, const Entity e2, const float step)
{
    if (e1 == e2)
        return FindSelfIntersection(e1, step);

    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));

    const auto surface1 = GetSurface(e1);
    const auto surface2 = GetSurface(e2);

    const auto firstApprox = FindFirstApproximation(*surface1, *surface2);
    const auto firstPointOpt = FindFirstIntersectionPoint(*surface1, *surface2, firstApprox);

    if (!firstPointOpt.has_value()) {
        std::cout << "Cannot find first point\n";
        return std::nullopt;
    }

    return FindIntersection(*surface1, *surface2, firstPointOpt.value(), step);
}


std::optional<Entity> IntersectionSystem::FindIntersection(const Entity e1, const Entity e2, const float step, const Position &guidance)
{
    if (e1 == e2)
        return FindSelfIntersection(e1, step, guidance);

    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));

    const auto surface1 = GetSurface(e1);
    const auto surface2 = GetSurface(e2);

    auto [initU, initV] = NearestPointApproximation(*surface1, guidance);
    const auto nearestPoint1 = NearestPoint(*surface1, guidance, initU, initV);

    std::tie(initU, initV) = NearestPointApproximation(*surface2, guidance);
    const auto nearestPoint2 = NearestPoint(*surface2, guidance, initU, initV);
    if (!nearestPoint1.has_value() || !nearestPoint2.has_value()) {
        std::cout << "Cannot find nearest point\n";
        return std::nullopt;
    }

    auto [u1, v1] = nearestPoint1.value();
    auto [u2, v2] = nearestPoint2.value();

    const IntersectionPoint startingApprox(u1, v1, u2, v2);

    const auto firstPointOpt = FindFirstIntersectionPoint(*surface1, *surface2, startingApprox);
    if (!firstPointOpt.has_value()) {
        std::cout << "Cannot find first point\n";
        return std::nullopt;
    }

    return FindIntersection(*surface1, *surface2, firstPointOpt.value(), step);
}


bool CheckInitialPointSelfIntersection(const IntersectionPoint& p) {
    constexpr float minDist = 0.05f;

    return DistanceSquared(alg::Vec2(p.U1(), p.V1()), alg::Vec2(p.U2(), p.V2())) > minDist*minDist;
}


std::optional<Entity> IntersectionSystem::FindSelfIntersection(const Entity e, const float step)
{
    assert(CanBeIntersected(e));

    const auto surface = GetSurface(e);

    const auto firstApprox = FindFirstApproximationForSelfIntersection(*surface);
    const auto firstPointOpt = FindFirstIntersectionPoint(*surface, *surface, firstApprox);

    if (!firstPointOpt.has_value() || !CheckInitialPointSelfIntersection(firstPointOpt.value())) {
        std::cout << "Cannot find first point\n";
        return std::nullopt;
    }

    return FindIntersection(*surface, *surface, firstPointOpt.value(), step);
}


std::optional<Entity> IntersectionSystem::FindSelfIntersection(const Entity e, const float step, const Position &guidance)
{
    assert(CanBeIntersected(e));

    const auto surface = GetSurface(e);

    auto [initU, initV] = NearestPointApproximation(*surface, guidance);
    const auto nearestPoint1 = NearestPoint(*surface, guidance, initU, initV);
    if (!nearestPoint1.has_value()) {
        std::cout << "Cannot find nearest point 1\n";
        return std::nullopt;
    }

    std::tie(initU, initV) = SecondNearestPointApproximation(
        *surface, guidance, std::get<0>(nearestPoint1.value()), std::get<1>(nearestPoint1.value())
    );
    const auto nearestPoint2 = NearestPoint(*surface, guidance, initU, initV);
    if (!nearestPoint1.has_value()) {
        std::cout << "Cannot find nearest point 2\n";
        return std::nullopt;
    }

    const IntersectionPoint initInterPoint(
        std::get<0>(nearestPoint1.value()),
        std::get<1>(nearestPoint1.value()),
        std::get<0>(nearestPoint2.value()),
        std::get<1>(nearestPoint2.value())
    );

    const auto firstInterPoint = FindFirstIntersectionPoint(*surface, *surface, initInterPoint);

    if (!firstInterPoint.has_value() || !CheckInitialPointSelfIntersection(firstInterPoint.value())) {
        std::cout << "Cannot find nearest point\n";
        return std::nullopt;
    }

    return FindIntersection(*surface, *surface, firstInterPoint.value(), step);
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


IntersectionPoint IntersectionSystem::FindFirstApproximationForSelfIntersection(Surface &s) const
{
    constexpr int sampleCntInOneDim = 40;
    constexpr float penaltyCoef = 0.5f;

    const float maxU = s.MaxUInitSampleVal();
    const float minU = s.MinUInitSampleVal();

    const float maxV = s.MaxVInitSampleVal();
    const float minV = s.MinVInitSampleVal();

    const float deltaU = (maxU - minU) / static_cast<float>(sampleCntInOneDim + 1);
    const float deltaV = (maxV - minV) / static_cast<float>(sampleCntInOneDim + 1);

    float minDist = std::numeric_limits<float>::infinity();
    IntersectionPoint result;

    for (int i = 1; i <= sampleCntInOneDim; ++i) {
        const float u1 = deltaU * static_cast<float>(i) + minU;

        for (int j = 1; j <= sampleCntInOneDim; ++j) {
            const float v1 = deltaV * static_cast<float>(j) + minV;

            for (int k = 1; k <= sampleCntInOneDim; ++k) {
                const float u2 = deltaU * static_cast<float>(k) + minU;

                for (int l = 1; l <= sampleCntInOneDim; ++l) {
                    const float v2 = deltaV * static_cast<float>(l) + minV;

                    auto point1 = s.PointOnSurface(u1, v1);
                    auto point2 = s.PointOnSurface(u2, v2);

                    float dist = Distance(point1, point2);
                    const float penalty = -penaltyCoef * Distance(alg::Vec2(u1, v1), alg::Vec2(u2, v2));
                    dist += penalty;

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


class DomainDichotomyLineSearch4D final : public opt::DichotomyLineSearch {
public:
    DomainDichotomyLineSearch4D(Surface& s1, Surface& s2, const float eps):
        DichotomyLineSearch(0.f, 1.f, eps), surface1(s1), surface2(s2) {}

    float Search(opt::FunctionToOptimize &fun, const std::vector<float> &start, const std::vector<float> &direction) override {
        float len = 0.f;

        for (const auto dir: direction) {
            len += dir*dir;
        }

        len = std::sqrt(len);

        const float d1 = std::min(start[0] - surface1.MinU(), surface1.MaxU() - start[0]);
        const float d2 = std::min(start[1] - surface1.MinV(), surface1.MaxV() - start[1]);
        const float d3 = std::min(start[2] - surface2.MinU(), surface2.MaxU() - start[2]);
        const float d4 = std::min(start[3] - surface2.MinV(), surface2.MaxV() - start[3]);

        const float minDist = std::min(std::min(d1, d2), std::min(d3, d4));

        if (minDist < len) {
            // Scale direction
            const float coef = minDist / len;

            const std::vector newDir {
                direction[0] * coef,
                direction[1] * coef,
                direction[2] * coef,
                direction[3] * coef
            };

            return coef * DichotomyLineSearch::Search(fun, start, newDir);
        }

        return DichotomyLineSearch::Search(fun, start, direction);
    }

private:
    Surface& surface1;
    Surface& surface2;
};


class DomainDichotomyLineSearch2D final : public opt::DichotomyLineSearch {
public:
    DomainDichotomyLineSearch2D(Surface& s1, const float eps):
        DichotomyLineSearch(0.f, 1.f, eps), surface1(s1) {}

    float Search(opt::FunctionToOptimize &fun, const std::vector<float> &start, const std::vector<float> &direction) override {
        float len = 0.f;

        for (const auto dir: direction) {
            len += dir*dir;
        }

        len = std::sqrt(len);

        const float d1 = std::min(start[0] - surface1.MinU(), surface1.MaxU() - start[0]);
        const float d2 = std::min(start[1] - surface1.MinV(), surface1.MaxV() - start[1]);

        const float minDist = std::min(d1, d2);

        if (minDist < len) {
            // Scale direction
            const float coef = minDist / len;

            const std::vector newDir {
                direction[0] * coef,
                direction[1] * coef,
            };

            return coef * DichotomyLineSearch::Search(fun, start, newDir);
        }

        return DichotomyLineSearch::Search(fun, start, direction);
    }

private:
    Surface& surface1;
};


std::optional<IntersectionPoint> IntersectionSystem::FindFirstIntersectionPoint(Surface& s1, Surface& s2, const IntersectionPoint& initSol) const
{
    const std::vector startingPoint = {
        initSol.U1(),
        initSol.V1(),
        initSol.U2(),
        initSol.V2()
    };

    DomainDichotomyLineSearch4D lineSearch(s1, s2, 1e-7f);
    NearZeroCondition stopCond;
    DistanceBetweenPoints fun(s1, s2);

    const auto sol = ConjugateGradientMethod(fun, lineSearch, startingPoint, 200, stopCond);

    if (!sol.has_value())
        return std::nullopt;

    IntersectionPoint result(
        sol.value()[0],
        sol.value()[1],
        sol.value()[2],
        sol.value()[3]
    );

    if (!PointInDomains(s1, s2, result))
        return std::nullopt;

    if (ErrorRate(s1, s2, result) > 1e-5)
        return std::nullopt;

    return result;
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


std::optional<std::tuple<float, float>> IntersectionSystem::NearestPoint(
    Surface &s, const Position &guidance, const float initU, const float initV) const
{
    const std::vector startingPoint {
        initU, initV
    };

    DomainDichotomyLineSearch2D lineSearch(s, 1e-7f);
    opt::SmallGradient stopCond;
    NearestPointFun fun(s, guidance);

    const auto solOpt = ConjugateGradientMethod(fun, lineSearch, startingPoint, 200, stopCond);
    if (!solOpt.has_value())
        return std::nullopt;

    const auto& sol = solOpt.value();

    if (!PointInDomain(s, sol.at(0), sol.at(1)))
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


std::tuple<float, float> IntersectionSystem::SecondNearestPointApproximation(
    Surface &s, const Position &guidance, const float firstU, const float firstV) const
{
    constexpr int sampleCntInOneDim = 30;
    constexpr float penaltyCoef = 0.5f;

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

            float dist = DistanceSquared(point, guidance.vec);
            const float penalty = -penaltyCoef * Distance(alg::Vec2(u, v), alg::Vec2(firstU, firstV));
            dist += penalty;

            if (minDist > dist) {
                minDist = dist;

                resultU = u;
                resultV = v;
            }
        }
    }

    return {resultU, resultV};
}


std::optional<Entity> IntersectionSystem::FindIntersection(Surface &s1, Surface &s2, const IntersectionPoint &initPoint, const float step)
{
    std::deque<IntersectionPoint> intersections;

    intersections.emplace_back(initPoint);
    const auto firstPoint = s1.PointOnSurface(initPoint.U1(), initPoint.V1());

    NextPointFinder nextPointFinder(s1, s2, initPoint, step);

    if (!nextPointFinder.FindNext()) {
        std::cout << "Cannot find second point\n";
        return std::nullopt;
    }

    const auto& secondInterPoint = nextPointFinder.ActualPoint();
    intersections.emplace_back(secondInterPoint);

    if (nextPointFinder.WasLastPoint())
        return FindOpenIntersection(initPoint, s1, s2, step, intersections);

    Position newPoint;
    do {
        if (!nextPointFinder.FindNext()) {
            std::cout << "Cannot find next point\n";
            return std::nullopt;
        }

        const auto& nextInterPoint = nextPointFinder.ActualPoint();
        intersections.emplace_back(nextInterPoint);

        newPoint = s1.PointOnSurface(nextInterPoint.U1(), nextInterPoint.V1());

        if (nextPointFinder.WasLastPoint())
            return FindOpenIntersection(initPoint, s1, s2, step, intersections);

    } while (DistanceSquared(firstPoint, newPoint.vec) > step*step);

    return CreateCurve(s1, s2, intersections, false);
}


std::optional<Entity> IntersectionSystem::FindOpenIntersection(
    const IntersectionPoint& firstPoint,
    Surface& s1,
    Surface& s2,
    const float step,
    std::deque<IntersectionPoint>& points
) {
    const IntersectionPoint prevSol(firstPoint.U2(), firstPoint.V2(), firstPoint.U1(), firstPoint.V1());

    // Passing solutions in reversed order to traverse intersection in other direction
    NextPointFinder nextPointFinder(s2, s1, prevSol, step);

    do {
        if (!nextPointFinder.FindNext()) {
            std::cout << "Cannot find first point\n";
            return std::nullopt;
        }

        const auto& sol = nextPointFinder.ActualPoint();
        points.emplace_front(sol.U2(), sol.V2(), sol.U1(), sol.V1());

    } while (!nextPointFinder.WasLastPoint());

    return CreateCurve(s1, s2, points, true);
}


float IntersectionSystem::ErrorRate(Surface& s1, Surface& s2, const IntersectionPoint &intPt) const
{
    const auto point1 = s1.PointOnSurface(intPt.U1(), intPt.V1());
    const auto point2 = s2.PointOnSurface(intPt.U2(), intPt.V2());

    return DistanceSquared(point1, point2);
}


Entity IntersectionSystem::CreateCurve(Surface& s1, Surface& s2, const std::deque<IntersectionPoint> &interPoints, const bool isOpen)
{
    std::vector<Entity> controlPoints;
    controlPoints.reserve(interPoints.size());

    for (auto point: interPoints) {
        const Entity cp = coordinator->CreateEntity();

        coordinator->AddComponent<Position>(cp, s1.PointOnSurface(point.U1(), point.V1()));

        controlPoints.push_back(cp);
    }

    if (!isOpen) {
        controlPoints.push_back(*controlPoints.begin());
    }

    const auto sys = coordinator->GetSystem<InterpolationCurvesRenderingSystem>();
    const Entity curve = sys->AddCurve(controlPoints);

    const auto handler = std::make_shared<DeletionHandler>(*coordinator);
    const auto handlerId = coordinator->Subscribe(curve, std::static_pointer_cast<EventHandler<CurveControlPoints>>(handler));

    IntersectionCurve interCurve(interPoints, isOpen, handlerId);
    for (auto& point : interCurve.intersectionPoints) {
        s1.Normalize(point.U1(), point.V1());
        s2.Normalize(point.U2(), point.V2());
    }

    coordinator->AddComponent<IntersectionCurve>(curve, interCurve);
    entities.insert(curve);

    return curve;
}


void IntersectionSystem::DeletionHandler::HandleEvent(
    Entity entity, const CurveControlPoints& component, const EventType eventType
) {
    if (eventType != EventType::ComponentDeleted)
        return;

    const auto points = component.GetPoints();
    const std::set toDelete(points.begin(), points.end());

    for (const auto cp : toDelete) {
        coordinator.DestroyEntity(cp);
    }
}



