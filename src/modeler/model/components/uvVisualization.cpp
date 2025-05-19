#include <CAD_modeler/model/components/uvVisualization.hpp>

#include <CAD_modeler/utilities/lineDrawer.hpp>


UvVisualization::UvVisualization(const size_t uRes, const size_t vRes):
    textureData(vRes, uRes, 0.f),
    texture(uRes, vRes, textureData.Data(), Texture2D::Red8BitFloat, Texture2D::Red)
{
}


void UvVisualization::DrawLine(const std::vector<alg::IVec2> &points) {
    for (size_t i = 0; i < points.size()-1; ++i) {
        const auto drawer = GetLineDrawer(points[i], points[i+1]);

        do {
            const auto u = Wrap(drawer->ActualPoint().Y(), VResolution());
            const auto v = Wrap(drawer->ActualPoint().X(), UResolution());

            textureData.At(
                u, v
            ) = 0.5f;
        } while (drawer->NextPoint());
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
