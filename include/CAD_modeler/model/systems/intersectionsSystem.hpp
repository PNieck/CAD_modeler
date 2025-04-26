#pragma once

#include <ecs/system.hpp>

#include <optional>
#include <memory>
#include <tuple>

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
    std::optional<interSys::IntersectionPoint> FindFirstIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, const interSys::IntersectionPoint& initSol) const;

    std::tuple<std::optional<interSys::IntersectionPoint>, bool> FindNextIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, interSys::IntersectionPoint &prevSol, float step) const;

    void FindOpenIntersection(const interSys::IntersectionPoint& firstPoint, interSys::Surface& s1, interSys::Surface& s2, float step) const;

    static bool SolutionInDomains(const interSys::IntersectionPoint &sol, interSys::Surface& s1, interSys::Surface& s2);
    static bool SolutionInDomain(interSys::Surface& s, float u, float v);

    float ErrorRate(interSys::Surface& s1, interSys::Surface& s2, const interSys::IntersectionPoint &intPt) const;
    float ErrorRate(Entity e1, Entity e2, const interSys::IntersectionPoint &intPt) const;

    friend class NextPointDistFun;
};
