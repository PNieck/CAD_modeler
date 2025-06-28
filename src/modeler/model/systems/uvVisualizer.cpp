#include <CAD_modeler/model/systems/uvVisualizer.hpp>

#include <CAD_modeler/model/systems/utils/getSurfaceSystem.hpp>

#include <CAD_modeler/model/components/uvVisualization.hpp>
#include "CAD_modeler/model/components/wraps.hpp"

#include <CAD_modeler/utilities/wrapToRange.hpp>


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


void UvVisualizer::DrawPoint(Entity e, size_t u, size_t v)
{
    coordinator.EditComponent<UvVisualization>(e,
        [u, v] (UvVisualization& vis) {
            vis.DrawPoint(u, v);
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

    coordinator.EditComponent<UvVisualization>(e,
        [&points] (UvVisualization& vis) {
            vis.DrawLine(points);
        }
    );

    if (!curve.isOpen) {
        const auto& firstPoint = curve.intersectionPoints[0];
        const auto& lastPoint = curve.intersectionPoints[curve.intersectionPoints.size() - 1];

        float firstU = firstPoint.AsVector()[i];
        float firstV = firstPoint.AsVector()[j];

        float lastU = lastPoint.AsVector()[i];
        float lastV = lastPoint.AsVector()[j];

        const bool firstNorm = PointIsNormalised(*sys, e, firstU, firstV);
        const bool lastNorm = PointIsNormalised(*sys, e, lastU, lastV);

        if (firstNorm != lastNorm) {
            if (firstNorm)
                NormalizePoint(*sys, e, firstU, firstV);
            else
                NormalizePoint(*sys, e, lastU, lastV);
        }

        int firstUI = static_cast<int>(std::round(firstU / maxU * uRes));
        int firstVI = static_cast<int>(std::round(firstV / maxV * vRes));

        int lastUI = static_cast<int>(std::round(lastU / maxU * uRes));
        int lastVI = static_cast<int>(std::round(lastV / maxV * vRes));

        points.clear();
        points.reserve(2);
        points.emplace_back(firstUI, firstVI);
        points.emplace_back(lastUI, lastVI);

        coordinator.EditComponent<UvVisualization>(e,
            [&points] (UvVisualization& vis) {
                vis.DrawLine(points);
            }
        );
    }
}


bool UvVisualizer::PointIsNormalised(const SurfaceSystem &sys, const Entity e, const float u, const float v) const
{
    return u >= 0.f && u <= sys.MaxU(e) && v >= 0.f && v <= sys.MaxV(e);
}


void UvVisualizer::NormalizePoint(const SurfaceSystem &sys, const Entity e, float &u, float &v) const
{
    u = WrapToRange(u, sys.MaxU(e));
    v = WrapToRange(v, sys.MaxV(e));
}
