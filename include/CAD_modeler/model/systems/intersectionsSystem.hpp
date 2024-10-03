#pragma once

#include <ecs/system.hpp>

#include <optional>

#include "algebra/vec4.hpp"


class IntersectionSystem: public System {
public:
    static void RegisterSystem(Coordinator& coordinator);

    bool CanBeIntersected(Entity entity) const;

    void FindIntersection(Entity e1, Entity e2);

private:
    class Solution {
    public:
        Solution(float v1, float u1, float v2, float u2):
            vec(v1, u1, v2, u2) {}

        Solution(alg::Vec4&& vec):
            vec(vec) {}

        inline float& V1()
            { return vec.X(); }

        inline float& U1()
            { return vec.Y(); }

        inline float& V2()
            { return vec.Z(); }

        inline float& U2()
            { return vec.W(); }

        inline alg::Vec4& AsVector()
            { return vec; }

    private:
        alg::Vec4 vec;
    };

    std::optional<Solution> FindFirstIntersectionPoint(Entity e1, Entity e2);

    bool CheckIfSolutionIsInDomain(Solution &sol) const;
};
