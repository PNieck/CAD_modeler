#pragma once

#include <algebra/vec2.hpp>

#include "texture2D.hpp"
#include "../../utilities/flatVec2D.hpp"


class UvVisualization {
public:
    UvVisualization(size_t uRes, size_t vRes);

    void DrawLine(const std::vector<alg::IVec2>& points);

    [[nodiscard]]
    size_t UResolution() const
        { return textureData.Cols(); }

    [[nodiscard]]
    size_t VResolution() const
        { return textureData.Rows(); }

    [[nodiscard]]
    unsigned int TextureId() const
        { return texture.Id(); }

private:
    FlatVec2D<float> textureData;
    Texture2D texture;

    static size_t Wrap(int coord, size_t resolution);
};
