#pragma once

#include <ecs/system.hpp>

#include <optional>
#include <memory>
#include <tuple>
#include <deque>

#include "CAD_modeler/model/components/position.hpp"
#include "CAD_modeler/model/components/intersectionCurve.hpp"
#include "intersectionSystem/surfaces.hpp"


class IntersectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    bool CanBeIntersected(Entity entity) const;

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step);

    std::optional<Entity> FindIntersection(Entity e1, Entity e2, float step, const Position& guidance);

    std::optional<Entity> FindSelfIntersection(Entity e, float step);

    std::optional<Entity> FindSelfIntersection(Entity e, float step, const Position& guidance);

private:
    [[nodiscard]]
    std::unique_ptr<interSys::Surface> GetSurface(Entity entity) const;

    [[nodiscard]]
    IntersectionPoint FindFirstApproximation(interSys::Surface& s1, interSys::Surface& s2) const;

    IntersectionPoint FindFirstApproximationForSelfIntersection(interSys::Surface& s) const;

    [[nodiscard]]
    std::optional<IntersectionPoint> FindFirstIntersectionPoint(interSys::Surface& s1, interSys::Surface& s2, const IntersectionPoint& initSol) const;

    std::optional<std::tuple<float, float>> NearestPoint(interSys::Surface& s, const Position& guidance, float initU, float initV) const;
    std::tuple<float, float> NearestPointApproximation(interSys::Surface& s, const Position& guidance) const;

    std::tuple<float, float> SecondNearestPointApproximation(interSys::Surface& s, const Position& guidance, float u, float v) const;

    std::optional<Entity> FindIntersection(interSys::Surface& s1, interSys::Surface& s2, const IntersectionPoint& initPoint, float step);

    std::optional<Entity> FindOpenIntersection(const IntersectionPoint& firstPoint, interSys::Surface& s1, interSys::Surface& s2, float step, std::deque<IntersectionPoint>& points) const;

    float ErrorRate(interSys::Surface& s1, interSys::Surface& s2, const IntersectionPoint &intPt) const;

    Entity CreateCurve(interSys::Surface& s1, interSys::Surface& s2, const std::deque<IntersectionPoint>& interPoints, bool isOpen) const;
};
