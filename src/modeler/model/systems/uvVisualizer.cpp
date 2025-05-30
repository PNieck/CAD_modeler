#include <CAD_modeler/model/systems/uvVisualizer.hpp>

#include <CAD_modeler/model/systems/utils/getSurfaceSystem.hpp>

#include <CAD_modeler/model/components/uvVisualization.hpp>
#include "CAD_modeler/model/components/wraps.hpp"


void UvVisualizer::VisualizeLineOnParameters(const Entity e, const IntersectionCurve &curve, const SurfaceCurveRelation relation)
{
    switch (relation) {
        case SurfaceCurveRelation::Surface1:
            DrawCurveOnUV(e, curve, 0, 1);
            break;

        case SurfaceCurveRelation::Surface2:
            DrawCurveOnUV(e, curve, 2, 3);
            break;

        case SurfaceCurveRelation::Both:
            DrawCurveOnUV(e, curve, 0, 1);
            DrawCurveOnUV(e, curve, 2, 3);
            break;

        default:
            throw std::runtime_error("Unknown surface curve relation");
    }
}


void UvVisualizer::FillTrimmingRegion(Entity e, size_t u, size_t v)
{
    coordinator.EditComponent<UvVisualization>(e,
        [this, u, v, e] (UvVisualization& vis) {
            vis.FillRegion(
                u, v,
                this->coordinator.HasComponent<WrapU>(e),
                this->coordinator.HasComponent<WrapV>(e)
            );
        }
    );
}


void UvVisualizer::DrawCurveOnUV(
    const Entity e, const IntersectionCurve &curve, const size_t i, const size_t j
) const {
    const auto sys = GetSurfaceSystem(coordinator, e);

    const float maxU = sys->MaxU(e);
    const float maxV = sys->MaxV(e);

    if (!coordinator.HasComponent<UvVisualization>(e)) {
        const UvVisualization vis(std::ceil(maxU * 100), std::ceil(maxV * 100));
        coordinator.AddComponent(e, vis);
    }

    auto const& uv = coordinator.GetComponent<UvVisualization>(e);

    const auto uRes = static_cast<float>(uv.UResolution());
    const auto vRes = static_cast<float>(uv.VResolution());

    std::vector<alg::IVec2> points;
    points.reserve(curve.intersectionPoints.size() + 1);

    for (auto const& point : curve.intersectionPoints) {
        int u = static_cast<int>(std::round(point.AsVector()[i] / maxU * uRes));
        int v = static_cast<int>(std::round(point.AsVector()[j] / maxV * vRes));

        points.emplace_back(u, v);
    }

    if (!curve.isOpen) {
        const auto& point = curve.intersectionPoints[0];

        int u = static_cast<int>(std::round(point.AsVector()[i] / maxU * uRes));
        int v = static_cast<int>(std::round(point.AsVector()[j] / maxV * vRes));

        points.emplace_back(u, v);
    }

    coordinator.EditComponent<UvVisualization>(e,
        [&points] (UvVisualization& vis) {
            vis.DrawLine(points);
        }
    );
}
