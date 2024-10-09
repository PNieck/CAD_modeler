#pragma once

#include <ecs/system.hpp>

#include <optional>

#include "algebra/vec4.hpp"


class IntersectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    bool CanBeIntersected(Entity entity) const;

    void FindIntersection(Entity e1, Entity e2, float step);

private:
    class IntersectionPoint {
    public:
        IntersectionPoint(float v1, float u1, float v2, float u2):
            vec(v1, u1, v2, u2) {}

        explicit IntersectionPoint(alg::Vec4&& vec):
            vec(vec) {}

        IntersectionPoint():
            IntersectionPoint(0.f, 0.f, 0.f, 0.f) {}

        inline float& V1()
            { return vec.X(); }

        inline float& U1()
            { return vec.Y(); }

        inline float& V2()
            { return vec.Z(); }

        inline float& U2()
            { return vec.W(); }

        inline float V1() const
            { return vec.X(); }

        inline float U1() const
            { return vec.Y(); }

        inline float V2() const
            { return vec.Z(); }

        inline float U2() const
            { return vec.W(); }

        inline alg::Vec4& AsVector()
            { return vec; }

    private:
        alg::Vec4 vec;
    };

    [[nodiscard]]
    IntersectionPoint FindFirstApproximation(Entity e1, Entity e2) const;

    [[nodiscard]]
    std::optional<IntersectionPoint> FindFirstIntersectionPoint(Entity e1, Entity e2, const IntersectionPoint& initSol) const;

    std::optional<IntersectionPoint> FindNextIntersectionPoint(Entity e1, Entity e2, IntersectionPoint &prevSol, float step) const;

    bool CheckIfSolutionIsInDomain(IntersectionPoint &sol) const;
    float ErrorRate(Entity e1, Entity e2, const IntersectionPoint &intPt) const;
};
