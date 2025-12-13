#pragma once

#include "../../utilities/flatVec2D.hpp"

#include <tuple>


class ModelHeightMap {
public:
    ModelHeightMap(const size_t xResolution, const size_t zResolution, const float xSize, const float zSize, const float defHeight = 0.f):
        defaultHeight(defHeight), xSize(xSize), zSize(zSize), data(xResolution ,zResolution){}

    [[nodiscard]]
    size_t XResolution() const
        { return data.Rows(); }

    [[nodiscard]]
    size_t ZResolution() const
        { return data.Cols(); }

    [[nodiscard]]
    float PixelXLen() const
        { return xSize / static_cast<float>(XResolution()); }

    [[nodiscard]]
    float PixelZLen() const
        { return zSize / static_cast<float>(ZResolution()); }

    [[nodiscard]]
    float MinX() const
        { return -MaxX(); }

    [[nodiscard]]
    float MinZ() const
        { return -MaxX(); }

    [[nodiscard]]
    float MaxX() const
        { return xSize / 2.f; }

    [[nodiscard]]
    float MaxZ() const
        { return zSize / 2.f; }

    float* Data()
        { return data.Data(); }

    [[nodiscard]]
    const FlatVec2D<float>& ToFlatVec2D() const
        { return data; }

    // Iterators
    [[nodiscard]]
    auto begin()
        { return data.begin(); }

    [[nodiscard]]
    auto end()
        { return data.end(); }

    [[nodiscard]]
    float Height(ssize_t x, ssize_t z) const;

    [[nodiscard]]
    float HeightFromGlobalCoordinates(float x, float z) const;

    [[nodiscard]]
    std::tuple<float, float> NearestPixelPoint(float x, float z) const;

    [[nodiscard]]
    std::tuple<ssize_t, ssize_t> GlobalPosToXY(float x, float z) const;


    float defaultHeight;

private:
    float xSize, zSize;
    FlatVec2D<float> data;
};
