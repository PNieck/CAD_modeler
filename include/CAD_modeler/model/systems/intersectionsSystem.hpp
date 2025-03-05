#pragma once

#include <ecs/system.hpp>

#include <optional>

#include <algebra/vec4.hpp>

#include "intersectionSystem/intersectionPoint.hpp"
#include "intersectionSystem/surfaces.hpp"


class IntersectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    bool CanBeIntersected(Entity entity) const;

    void FindIntersection(Entity e1, Entity e2, float step);

private:
    [[nodiscard]]
    std::unique_ptr<interSys::Surface> GetSurface(Entity entity) const;

    [[nodiscard]]
    interSys::IntersectionPoint FindFirstApproximation(interSys::Surface& s1, interSys::Surface& s2) const;

    [[nodiscard]]
    std::optional<interSys::IntersectionPoint> FindFirstIntersectionPointDLib(Entity e1, Entity e2, const interSys::IntersectionPoint& initSol) const;

    [[nodiscard]]
    std::optional<interSys::IntersectionPoint> FindFirstIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, const interSys::IntersectionPoint& initSol) const;

    std::optional<interSys::IntersectionPoint> FindNextIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, interSys::IntersectionPoint &prevSol, float step) const;

    bool CheckIfSolutionIsInDomain(interSys::IntersectionPoint &sol) const;
    float ErrorRate(interSys::Surface& s1, interSys::Surface& s2, const interSys::IntersectionPoint &intPt) const;
    float ErrorRate(Entity e1, Entity e2, const interSys::IntersectionPoint &intPt) const;
};
