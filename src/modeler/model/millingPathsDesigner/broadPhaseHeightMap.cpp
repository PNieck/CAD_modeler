#include <CAD_modeler/model/millingPathsDesigner/broadPhaseHeightMap.hpp>

#include <algorithm>
#include <cmath>


float BroadPhaseHeightMap::Height(const ssize_t x, const ssize_t z) const
{
    if (x < 0 || x >= XResolution() || z < 0 || z >= ZResolution())
        return defaultHeight;

    return std::max(data.At(x, z), defaultHeight);
}


float BroadPhaseHeightMap::HeightFromGlobalCoordinates(float x, float z) const
{
    auto [tileX, tileZ] = GlobalPosToXY(x, z);
    return Height(tileX, tileZ);
}


std::tuple<float, float> BroadPhaseHeightMap::NearestPixelPoint(const float x, const float z) const
{
    const float pixelXLen = PixelXLen();
    const float pixelZLen = PixelZLen();

    const float minX = MinX() + pixelXLen / 2.f;
    const float minZ = MinZ() + pixelZLen / 2.f;

    const float xTimes = std::round((x - minX) / pixelXLen);
    const float zTimes = std::round((z - minZ) / pixelZLen);

    float resX = xTimes * pixelXLen + minX;
    float resZ = zTimes * pixelZLen + minZ;

    return std::make_tuple(resX, resZ);
}


std::tuple<ssize_t, ssize_t> BroadPhaseHeightMap::GlobalPosToXY(const float x, const float z) const
{
    const float pixelXLen = PixelXLen();
    const float pixelZLen = PixelZLen();

    const float minX = MinX() + pixelXLen / 2.f;
    const float minZ = MinZ() + pixelZLen / 2.f;

    const auto resX = static_cast<ssize_t>(std::round((x - minX) / pixelXLen));
    const auto resZ = static_cast<ssize_t>(std::round((z - minZ) / pixelZLen));

    return std::make_tuple(resX, resZ);
}
