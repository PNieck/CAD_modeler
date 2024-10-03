#include <CAD_modeler/model/systems/intersectionsSystem.hpp>

#include <CAD_modeler/model/systems/c0PatchesSystem.hpp>
#include <CAD_modeler/model/systems/c2SurfacesSystem.hpp>
#include <CAD_modeler/model/systems/c2CylinderSystem.hpp>
#include <CAD_modeler/model/systems/toriSystem.hpp>
#include <CAD_modeler/model/systems/pointsSystem.hpp>

#include <ecs/coordinator.hpp>

#include <algebra/conjugateGradientMethod.hpp>

#include <dlib/optimization.h>

#include <cassert>
#include <numbers>

// TODO: remove
#include <iostream>


typedef dlib::matrix<double,0,1> columnVector;


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


void IntersectionSystem::FindIntersection(Entity e1, Entity e2)
{
    assert(CanBeIntersected(e1));
    assert(CanBeIntersected(e2));
    assert(e1 != e2);

    auto sol = FindFirstIntersectionPoint(e1, e2).value();

    const auto toriSys = coordinator->GetSystem<ToriSystem>();
    auto pointSys = coordinator->GetSystem<PointsSystem>();

    pointSys->CreatePoint(toriSys->PointOnTorus(e1, sol.V1(), sol.U1()));
    pointSys->CreatePoint(toriSys->PointOnTorus(e2, sol.V2(), sol.U2()));
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


class FunctionDer {
public:
    explicit FunctionDer(const std::shared_ptr<ToriSystem> &toriSys, Entity e1, Entity e2):
        toriSys(toriSys), e1(e1), e2(e2) {}

    const columnVector operator() (const columnVector& args) const
    {
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
        result(3) = -2.f * (xDiv*partDivU2.X() + yDiv*partDivU2.Y() + zDiv*partDivU2.Z());    // df/du2

        return result;
    }

private:
    std::shared_ptr<ToriSystem> toriSys;
    Entity e1, e2;
};


std::optional<IntersectionSystem::Solution> IntersectionSystem::FindFirstIntersectionPoint(Entity e1, Entity e2)
{
    columnVector startingPoint = {1, 1, 1, 1};

    Function fun(
        coordinator->GetSystem<ToriSystem>(),
        e1, e2
    );

    FunctionDer funDer(
        coordinator->GetSystem<ToriSystem>(),
        e1, e2
    );

    std::cout << "Difference between analytic derivative and numerical approximation of derivative: " 
         << dlib::length(dlib::derivative(fun)(startingPoint) - funDer(startingPoint)) << std::endl;

    std::cout << "Analytic der: " << funDer(startingPoint) << " numbric der: " << dlib::derivative(fun)(startingPoint) << std::endl;

    dlib::find_min(dlib::bfgs_search_strategy(),
                   dlib::objective_delta_stop_strategy(1e-7).be_verbose(),
                   fun, funDer, startingPoint, 0);

    return Solution(
        static_cast<float>(startingPoint(0)),
        static_cast<float>(startingPoint(1)),
        static_cast<float>(startingPoint(2)),
        static_cast<float>(startingPoint(3))
    );
}


bool IntersectionSystem::CheckIfSolutionIsInDomain(Solution &sol) const
{
    constexpr float maxVal = 2.0f * std::numbers::pi_v<float>;

    if (sol.V1() >= 0.f && sol.V1() <= maxVal &&
        sol.U1() >= 0.f && sol.U1() <= maxVal &&
        sol.V2() >= 0.f && sol.V2() <= maxVal &&
        sol.U2() >= 0.f && sol.U2() <= maxVal)
        return true;

    return false;
}
