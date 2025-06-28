#include <CAD_modeler/model/components/uvVisualization.hpp>

#include <CAD_modeler/utilities/lineDrawer.hpp>

#include <queue>


UvVisualization::UvVisualization(const size_t uRes, const size_t vRes):
    textureData(vRes, uRes, EmptyValue),
    texture(uRes, vRes, textureData.Data(), Texture2D::Red8BitFloat, Texture2D::Red)
{
}


void UvVisualization::DrawLine(const std::vector<alg::IVec2> &points) {
    for (size_t i = 0; i < points.size()-1; ++i) {
        const auto drawer = GetLineDrawer(points[i], points[i+1]);

        do {
            const auto u = Wrap(drawer->ActualPoint().Y(), VResolution());
            const auto v = Wrap(drawer->ActualPoint().X(), UResolution());

            textureData.At(u, v) = BorderValue;
        } while (drawer->NextPoint());
    }

    texture.Update(textureData.Data(), Texture2D::Red);
}


void UvVisualization::DrawPoint(const size_t u, const size_t v)
{
    textureData.At(v, u) = BorderValue;
    texture.Update(textureData.Data(), Texture2D::Red);
}


void UvVisualization::FillRegion(size_t u, size_t v, const bool wrapU, const bool wrapV)
{
    const float initVal = textureData.At(v, u);
    if (initVal == BorderValue)
        return;

    const float paintVal = initVal == EmptyValue ? FilledValue : EmptyValue;
    const float paintOverVal = paintVal == FilledValue ? EmptyValue :FilledValue;

    textureData.At(v, u) = paintVal;
    std::queue<UVPoint> toVisit;
    toVisit.emplace(u, v);

    while (!toVisit.empty()) {
        UVPoint actPoint = toVisit.front();
        toVisit.pop();

        // North direction
        auto neighbour = NorthNeighbour(actPoint, wrapU);
        if (neighbour.has_value())
            ProcessPoint(neighbour.value(), paintVal, paintOverVal, toVisit);

        // East direction
        neighbour = EastNeighbour(actPoint, wrapV);
        if (neighbour.has_value())
            ProcessPoint(neighbour.value(), paintVal, paintOverVal, toVisit);

        // South direction
        neighbour = SouthNeighbour(actPoint, wrapU);
        if (neighbour.has_value())
            ProcessPoint(neighbour.value(), paintVal, paintOverVal, toVisit);

        // West direction
        neighbour = WestNeighbour(actPoint, wrapV);
        if (neighbour.has_value())
            ProcessPoint(neighbour.value(), paintVal, paintOverVal, toVisit);
    }

    texture.Update(textureData.Data(), Texture2D::Red);
}


size_t UvVisualization::Wrap(int coord, const size_t resolution)
{
    if (coord < 0) {
        const auto resFloat = static_cast<float>(resolution);
        coord += static_cast<int>(std::ceil(static_cast<float>(-coord) / resFloat) * resFloat);
        return coord;
    }

    return coord % resolution;
}



std::optional<UvVisualization::UVPoint> UvVisualization::NorthNeighbour(const UVPoint &actPoint, const bool wrapU) const
{
    UVPoint result(actPoint.u + 1, actPoint.v);

    if (result.u < UResolution())
        return result;

    if (wrapU) {
        result.u = 0;
        return result;
    }

    return std::nullopt;
}


std::optional<UvVisualization::UVPoint> UvVisualization::EastNeighbour(const UVPoint &actPoint, const bool wrapV) const
{
    UVPoint result(actPoint.u, actPoint.v + 1);

    if (result.v < VResolution())
        return result;

    if (wrapV) {
        result.v = 0;
        return result;
    }

    return std::nullopt;
}


std::optional<UvVisualization::UVPoint> UvVisualization::SouthNeighbour(const UVPoint &actPoint, const bool wrapU) const
{
    if (actPoint.u > 0)
        return UVPoint(actPoint.u - 1, actPoint.v);

    if (wrapU)
        return UVPoint(UResolution() - 1, actPoint.v);

    return std::nullopt;
}


std::optional<UvVisualization::UVPoint> UvVisualization::WestNeighbour(const UVPoint &actPoint, const bool wrapV) const
{
    if (actPoint.v > 0)
        return UVPoint(actPoint.u, actPoint.v - 1);

    if (wrapV)
        return UVPoint(actPoint.u, VResolution() - 1);

    return std::nullopt;
}


void UvVisualization::ProcessPoint(const UVPoint &p, const float paintVal, const float paintOver, std::queue<UVPoint> &points)
{
    if (textureData.At(p.v, p.u) != paintOver)
        return;

    textureData.At(p.v, p.u) = paintVal;
    points.push(p);
}
