#include <CAD_modeler/model/systems/intersectionsSystem.hpp>

#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>
#include <CAD_modeler/model/systems/c2CylinderSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>
#include <CAD_modeler/model/systems/vectorSystem.hpp>

#include <ecs/coordinator.hpp>

#include <optimization/conjugateGradientMethod.hpp>
#include <optimization/lineSearchMethods/dichotomyLineSearch.hpp>

#include <rootFinding/newtonMethod.hpp>

#include <dlib/optimization.h>

#include <cassert>

// TODO: remove
#include <iostream>


typedef dlib::matrix<double,0,1> columnVector;


using namespace interSys;


void IntersectionSystem::RegisterSystem(Coordinator &coordinator)
{
    coordinator.RegisterSystem<IntersectionSystem>();
}


bool IntersectionSystem::CanBeIntersected(Entity entity) const
{
    // if (coordinator->GetSystem<C0PatchesSystem>()->GetEntities().contains(entity))
    //     return true;
    //
    // if (coordinator->GetSystem<C2SurfaceSystem>()->GetEntities().contains(entity))
    //     return true;
    //
    // if (coordinator->GetSystem<C2CylinderSystem>()->GetEntities().contains(entity))
    //     return true;

    if (coordinator->GetSystem<ToriSystem>()->GetEntities().contains(entity))
        return true;

    return false;
}


void IntersectionSystem::FindIntersection(Entity e1, Entity e2, float step)
{
    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));
    assert(e1 != e2);

    const auto pointSys = coordinator->GetSystem<PointsSystem>();

    auto surface1 = GetSurface(e1);
    auto surface2 = GetSurface(e2);

    const auto initSol = FindFirstApproximation(*surface1, *surface2);
    auto solOpt = FindFirstIntersectionPoint(*surface1, *surface2, initSol);
    if (!solOpt.has_value()) {
        std::cout << "WARNING: using dlib to calculate first intersection point" << std::endl;
        solOpt = FindFirstIntersectionPointDLib(e1, e2, initSol);
    }

    if (!solOpt.has_value()) {
        std::cout << "Cannot find first point\n";
        return;
    }

    auto& sol = solOpt.value();

    const auto firstPoint = surface1->PointOnSurface(sol.U1(), sol.V1());
    pointSys->CreatePoint(firstPoint);

    solOpt = FindNextIntersectionPoint(*surface1, *surface2, sol, step);

    if (!solOpt.has_value()) {
        std::cout << "Cannot find second point\n";
        return;
    }

    sol = solOpt.value();
    pointSys->CreatePoint(surface1->PointOnSurface(sol.U1(), sol.V1()));

    Position newPoint;
    do {
        solOpt = FindNextIntersectionPoint(*surface1, *surface2, sol, step);

        if (!solOpt.has_value()) {
            std::cout << "Cannot find next point\n";
            return;
        }

        sol = solOpt.value();
        pointSys->CreatePoint(surface1->PointOnSurface(sol.U1(), sol.V1()));

        newPoint = surface1->PointOnSurface(sol.U1(), sol.V1());
    } while (alg::DistanceSquared(firstPoint, newPoint.vec) > step*step);

}


std::unique_ptr<Surface> IntersectionSystem::GetSurface(const Entity entity) const
{
    if (coordinator->GetSystem<ToriSystem>()->GetEntities().contains(entity))
        return std::make_unique<TorusSurface>(*coordinator, entity);

    throw std::runtime_error("Entity cannot be used to calculate intersection curve");
}


class Function {
public:
    explicit Function(const std::shared_ptr<ToriSystem> &toriSys, Entity e1, Entity e2):
        toriSys(toriSys), e1(e1), e2(e2) {}

    double operator() (const columnVector& args) const
    {
        const auto point1 = toriSys->PointOnTorus(e1, args(0), args(1));
        const auto point2 = toriSys->PointOnTorus(e2, args(2), args(3));

        return alg::DistanceSquared(point1.vec, point2.vec);
    }

private:
    std::shared_ptr<ToriSystem> toriSys;
    Entity e1, e2;
};


IntersectionPoint IntersectionSystem::FindFirstApproximation(interSys::Surface& s1, interSys::Surface& s2) const
{
    constexpr int oneDimSamplesCnt = 15;

    const float maxU1 = s1.MaxU();
    const float maxV1 = s1.MaxV();
    const float maxU2 = s2.MaxU();
    const float maxV2 = s2.MaxV();

    const float deltaU1 = maxU1 / static_cast<float>(oneDimSamplesCnt + 1);
    const float deltaV1 = maxV1 / static_cast<float>(oneDimSamplesCnt + 1);
    const float deltaU2 = maxU2 / static_cast<float>(oneDimSamplesCnt + 1);
    const float deltaV2 = maxV2 / static_cast<float>(oneDimSamplesCnt + 1);

    float minDist = std::numeric_limits<float>::infinity();
    IntersectionPoint result;

    for (int i = 1; i <= oneDimSamplesCnt; ++i) {
        const float u1 = deltaU1 * static_cast<float>(i);

        for (int j = 1; j <= oneDimSamplesCnt; ++j) {
            const float v1 = deltaV1 * static_cast<float>(j);

            for (int k = 1; k <= oneDimSamplesCnt; ++k) {
                const float u2 = deltaU2 * static_cast<float>(k);

                for (int l = 1; l <= oneDimSamplesCnt; ++l) {
                    const float v2 = deltaV2 * static_cast<float>(l);

                    auto point1 = s1.PointOnSurface(u1, v1);
                    auto point2 = s2.PointOnSurface(u2, v2);

                    const float dist = alg::DistanceSquared(point1, point2);
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


class FunctionDer {
public:
    explicit FunctionDer(const std::shared_ptr<ToriSystem> &toriSys, Entity e1, Entity e2):
        toriSys(toriSys), e1(e1), e2(e2) {}

    columnVector operator()(const columnVector &args) const {
        const auto point1 = toriSys->PointOnTorus(e1, args(0), args(1));
        const auto point2 = toriSys->PointOnTorus(e2, args(2), args(3));

        const auto partDivV1 = toriSys->PartialDerivativeWithRespectToAlpha(e1, args(0), args(1));
        const auto partDivV2 = toriSys->PartialDerivativeWithRespectToAlpha(e2, args(2), args(3));

        const auto partDivU1 = toriSys->PartialDerivativeWithRespectToBeta(e1, args(0), args(1));
        const auto partDivU2 = toriSys->PartialDerivativeWithRespectToBeta(e2, args(2), args(3));

        const float xDiv = point1.GetX() - point2.GetX();
        const float yDiv = point1.GetY() - point2.GetY();
        const float zDiv = point1.GetZ() - point2.GetZ();

        columnVector result(4);

        result(0) = 2.f * (xDiv*partDivV1.X() + yDiv*partDivV1.Y() + zDiv*partDivV1.Z());   // df/dv1
        result(1) = 2.f * (xDiv*partDivU1.X() + yDiv*partDivU1.Y() + zDiv*partDivU1.Z());   // df/du1
        result(2) = -2.f * (xDiv*partDivV2.X() + yDiv*partDivV2.Y() + zDiv*partDivV2.Z());  // df/dv2
        result(3) = -2.f * (xDiv*partDivU2.X() + yDiv*partDivU2.Y() + zDiv*partDivU2.Z());  // df/du2

        return result;
    }

private:
    std::shared_ptr<ToriSystem> toriSys;
    Entity e1, e2;
};


class DistanceBetweenPoints: public opt::FunctionToOptimize {
public:
    explicit DistanceBetweenPoints(Surface& s1, Surface& s2):
        surface1(s1), surface2(s2) {}

    float Value(const std::vector<float> &args) override {
        const auto point1 = surface1.PointOnSurface(args[0], args[1]);
        const auto point2 = surface2.PointOnSurface(args[2], args[3]);

        return alg::DistanceSquared(point1, point2);
    }


    std::vector<float> Gradient(const std::vector<float> &args) override {
        const auto point1 = surface1.PointOnSurface(args[0], args[1]);
        const auto point2 = surface2.PointOnSurface(args[2], args[3]);

        const auto partDivU1 = surface1.PartialDerivativeU(args[0], args[1]);
        const auto partDivU2 = surface2.PartialDerivativeU(args[2], args[3]);

        const auto partDivV1 = surface1.PartialDerivativeV(args[0], args[1]);
        const auto partDivV2 = surface2.PartialDerivativeV(args[2], args[3]);

        const float xDiv = point1.X() - point2.X();
        const float yDiv = point1.Y() - point2.Y();
        const float zDiv = point1.Z() - point2.Z();

        std::vector<float> result(4);

        result[0] = 2.f * (xDiv*partDivU1.X() + yDiv*partDivU1.Y() + zDiv*partDivU1.Z());   // df/du1
        result[1] = 2.f * (xDiv*partDivV1.X() + yDiv*partDivV1.Y() + zDiv*partDivV1.Z());   // df/dv1
        result[2] = -2.f * (xDiv*partDivU2.X() + yDiv*partDivU2.Y() + zDiv*partDivU2.Z());  // df/du2
        result[3] = -2.f * (xDiv*partDivV2.X() + yDiv*partDivV2.Y() + zDiv*partDivV2.Z());  // df/dv2

        return result;
    }

private:
    Surface& surface1;
    Surface& surface2;
};


std::optional<IntersectionPoint> IntersectionSystem::FindFirstIntersectionPointDLib(Entity e1, Entity e2,
    const IntersectionPoint& initSol) const
{
    columnVector startingPoint = {
        initSol.U1(),
        initSol.V1(),
        initSol.U2(),
        initSol.V2()
    };

    const auto toriSys = coordinator->GetSystem<ToriSystem>();

    const Function fun(toriSys, e1, e2);
    const FunctionDer funDer(toriSys, e1, e2);

    dlib::find_min(dlib::bfgs_search_strategy(),
                   dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
                   fun, funDer, startingPoint, 0);


    IntersectionPoint result(
        static_cast<float>(startingPoint(0)),
        static_cast<float>(startingPoint(1)),
        static_cast<float>(startingPoint(2)),
        static_cast<float>(startingPoint(3))
    );

    if (ErrorRate(e1, e2, result) > 1e-5)
        return std::nullopt;

    return result;
}


std::optional<IntersectionPoint> IntersectionSystem::FindFirstIntersectionPoint(Surface& s1, Surface& s2, const IntersectionPoint& initSol) const
{
    const std::vector<float> startingPoint = {
        initSol.U1(),
        initSol.V1(),
        initSol.U2(),
        initSol.V2()
    };

    opt::DichotomyLineSearch lineSearch(0, 1.f, 1e-7f);
    DistanceBetweenPoints fun(s1, s2);

    const auto sol = opt::ConjugateGradientMethod(fun, lineSearch, startingPoint, 1e-7, 100);

    if (!sol.has_value())
        return std::nullopt;

    IntersectionPoint result(
        sol.value()[0],
        sol.value()[1],
        sol.value()[2],
        sol.value()[3]
    );

    if (ErrorRate(s1, s2, result) > 1e-5)
        return std::nullopt;

    return result;
}


class NextPointDistFun final : public root::FunctionToFindRoot {
public:
    NextPointDistFun(Surface& s1, Surface& s2, const alg::Vec3& prevPoint,
                     const alg::Vec3& tangent, float step):
        surface1(s1), surface2(s2), prevPoint(prevPoint), tangent(tangent), step(step) {}

    alg::Vec4 Value(alg::Vec4 args) override {
        const auto point1 = surface1.PointOnSurface(args.X(), args.Y());
        const auto point2 = surface2.PointOnSurface(args.Z(), args.W());

        auto diff = point1 - point2;
        auto v = alg::Dot(point1 - prevPoint, tangent) - step;

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

        const float partX = alg::Dot(tangent, derE1X);
        const float partY = alg::Dot(tangent, derE1Y);

        auto mtx = alg::Mat4x4(
            derE1X.X(), derE1Y.X(), -derE2Z.X(), -derE2W.X(),
            derE1X.Y(), derE1Y.Y(), -derE2Z.Y(), -derE2W.Y(),
            derE1X.Z(), derE1Y.Z(), -derE2Z.Z(), -derE2W.Z(),
                 partX,      partY,         0.f,         0.f
        );

        return mtx;
    }

private:
    Surface& surface1;
    Surface& surface2;

    alg::Vec3 prevPoint;
    alg::Vec3 tangent;
    float step;
};


std::optional<IntersectionPoint> IntersectionSystem::FindNextIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, IntersectionPoint &prevSol, float step) const
{
    float remainingDist = step;
    const float minStep = step / 1024.f;
    std::optional<alg::Vec4> nextPoint;

    do {
        alg::Vec3 normal1 = s1.NormalVector(prevSol.U1(), prevSol.V1());
        alg::Vec3 normal2 = s2.NormalVector(prevSol.U2(), prevSol.V2());

        alg::Vec3 tangent = alg::Cross(normal1, normal2);
        alg::Vec3 prevPoint = s1.PointOnSurface(prevSol.U1(), prevSol.V1());

        NextPointDistFun fun(
            s1, s2,
            prevPoint,
            tangent,
            step
        );

        nextPoint = root::NewtonMethod(fun, prevSol.AsVector(), 1e-5);
        if (!nextPoint.has_value()) {
            step /= 2.f;
            if (step < minStep)
                return std::nullopt;
        }
        else {
            remainingDist -= step;
            step = remainingDist;
        }

    } while (remainingDist > 0.f);



    return IntersectionPoint(
        nextPoint.value().X(),
        nextPoint.value().Y(),
        nextPoint.value().Z(),
        nextPoint.value().W()
    );
}


bool IntersectionSystem::CheckIfSolutionIsInDomain(IntersectionPoint &sol) const
{
    constexpr float maxVal = 2.0f * std::numbers::pi_v<float>;

    if (sol.V1() >= 0.f && sol.V1() <= maxVal &&
        sol.U1() >= 0.f && sol.U1() <= maxVal &&
        sol.V2() >= 0.f && sol.V2() <= maxVal &&
        sol.U2() >= 0.f && sol.U2() <= maxVal)
        return true;

    return false;
}


float IntersectionSystem::ErrorRate(Surface& s1, Surface& s2, const IntersectionPoint &intPt) const
{
    const auto point1 = s1.PointOnSurface(intPt.U1(), intPt.V1());
    const auto point2 = s2.PointOnSurface(intPt.U2(), intPt.V2());

    return alg::DistanceSquared(point1, point2);
}


float IntersectionSystem::ErrorRate(Entity e1, Entity e2, const IntersectionPoint &intPt) const
{
    const auto toriSys = coordinator->GetSystem<ToriSystem>();

    const auto point1 = toriSys->PointOnTorus(e1, intPt.V1(), intPt.U1());
    const auto point2 = toriSys->PointOnTorus(e2, intPt.V2(), intPt.U2());

    return alg::DistanceSquared(point1.vec, point2.vec);
}
