#pragma once

#include <algebra/vec2.hpp>

#include "texture2D.hpp"
#include "../../utilities/flatVec2D.hpp"

#include <optional>
#include <queue>


class UvVisualization {
public:
    UvVisualization(size_t uRes, size_t vRes);

    void DrawLine(const std::vector<alg::IVec2>& points);

    void DrawPoint(size_t u, size_t v);

    void FillRegion(size_t u, size_t v, bool wrapU, bool wrapV);

    [[nodiscard]]
    size_t UResolution() const
        { return textureData.Cols(); }

    [[nodiscard]]
    size_t VResolution() const
        { return textureData.Rows(); }

    [[nodiscard]]
    unsigned int TextureId() const
        { return texture.Id(); }

    void UseTexture() const
        { texture.Use(); }

private:
    static constexpr float BorderValue = 0.5f;
    static constexpr float EmptyValue = 0.f;
    static constexpr float FilledValue = 1.f;

    FlatVec2D<float> textureData;
    Texture2D texture;

    struct UVPoint {
        UVPoint(const size_t u, const size_t v):
            u(u), v(v) {}

        size_t u, v;
    };

    static size_t Wrap(int coord, size_t resolution);

    [[nodiscard]]
    std::optional<UVPoint> NorthNeighbour(const UVPoint& actPoint, bool wrapU) const;
    std::optional<UVPoint> EastNeighbour(const UVPoint& actPoint, bool wrapV) const;
    std::optional<UVPoint> SouthNeighbour(const UVPoint& actPoint, bool wrapU) const;
    std::optional<UVPoint> WestNeighbour(const UVPoint& actPoint, bool wrapV) const;


    void ProcessPoint(const UVPoint& p, float paintVal, float paintOver, std::queue<UVPoint>& points);
};
