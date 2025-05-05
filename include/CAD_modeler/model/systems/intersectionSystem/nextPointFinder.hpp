#pragma once

#include "surfaces.hpp"
#include "../../components/intersectionCurve.hpp"


namespace interSys {
    class NextPointFinder {
    public:
        NextPointFinder(Surface& s1, Surface& s2, const IntersectionPoint& firstPoint, float step);

        bool FindNext();

        [[nodiscard]]
        bool WasLastPoint() const
            { return wasLastPoint; }

        [[nodiscard]]
        const IntersectionPoint& ActualPoint() const
            { return actPoint; }

    private:
        Surface& surface1;
        Surface& surface2;

        alg::Vec3 actTangent;
        IntersectionPoint actPoint;
        float step;

        bool wasLastPoint = false;
        bool firstPoint = true;

        [[nodiscard]]
        alg::Vec3 Tangent(const IntersectionPoint& point) const;

        [[nodiscard]]
        bool ReverseTangent(const alg::Vec3& newTangent) const;

        void UpdateTangent();
    };
}

