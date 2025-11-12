#pragma once

#include "../../utilities/flatVec2D.hpp"

#include <tuple>


class BroadPhaseHeightMap {
public:
    BroadPhaseHeightMap(const size_t xResolution, const size_t zResolution, float xSize, float zSize, float defHeight = 0.f):
        defaultHeight(defHeight), xSize(xSize), zSize(zSize), data(xResolution ,zResolution){}

    size_t XResolution() const
        { return data.Rows(); }

    size_t ZResolution() const
        { return data.Cols(); }

    float PixelXLen() const
        { return xSize / static_cast<float>(XResolution()); }

    float PixelZLen() const
        { return zSize / static_cast<float>(ZResolution()); }

    float MinX() const
        { return -MaxX(); }

    float MinZ() const
        { return -MaxX(); }

    float MaxX() const
        { return xSize / 2.f; }

    float MaxZ() const
        { return zSize / 2.f; }

    float* Data()
        { return data.Data(); }

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

    float HeightFromGlobalCoordinates(float x, float z) const;

    std::tuple<float, float> NearestPixelPoint(float x, float z) const;

    std::tuple<ssize_t, ssize_t> GlobalPosToXY(float x, float z) const;


    float defaultHeight;

private:
    float xSize, zSize;
    FlatVec2D<float> data;
};
