#pragma once

#include "CAD_modeler/model/components/intersectionCurve.hpp"

#include "ecs/coordinator.hpp"

#include "CAD_modeler/model/components/uvVisualization.hpp"


class UvVisualizer {
public:
    enum class SurfaceCurveRelation {
        Surface1,
        Surface2,
        Both
    };

    explicit UvVisualizer(Coordinator& coordinator):
        coordinator(coordinator) {}

    void VisualizeLineOnParameters(Entity e, const IntersectionCurve& curve, SurfaceCurveRelation relation);

private:
    Coordinator& coordinator;

    void DrawCurveOnUV(Entity e, const IntersectionCurve& curve, size_t i, size_t j) const;
};
