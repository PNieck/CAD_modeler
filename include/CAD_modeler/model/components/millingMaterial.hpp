#pragma once

#include "texture2D.hpp"
#include "mesh.hpp"
#include "../systems/shaders/millingMaterial/millingMaterialTopShader.hpp"
#include "../systems/shaders/millingMaterial/millingMaterialSideShader.hpp"
#include "../systems/shaders/millingMaterial/millingMaterialBottomShader.hpp"

#include <algebra/vec3.hpp>
#include <algebra/mat4x4.hpp>


class MillingMaterial {
public:
    MillingMaterial(int xResolution, int zResolution, float xLen, float zLen, float thickness);

    [[nodiscard]]
    int XResolution() const
        { return heightMap.GetWidth(); }

    [[nodiscard]]
    int ZResolution() const
        { return heightMap.GetHeight(); }

    void SetResolution(int xRes, int zRes);

    void SetSize(float xLen, float zLen);

    void SetThickness(float thickness);

    void SetBaseLevel(float level);

    void Reset();

    [[nodiscard]]
    float XLength() const
        { return xLen; }

    [[nodiscard]]
    float ZLength() const
        { return zLen; }

    [[nodiscard]]
    float InitThickness() const
        { return initThickness; }

    [[nodiscard]]
    float BaseLevel() const
        { return mainHeightMapCorner.Y(); }

    float PixelXLen() const
        { return xLen / static_cast<float>(XResolution()); }

    float PixelZLen() const
        { return zLen / static_cast<float>(ZResolution()); }

    float MinX() const
        { return mainHeightMapCorner.X(); }

    float MinZ() const
        { return mainHeightMapCorner.Z(); }

    float GlobalX(int x) const;

    float GlobalZ(int z) const;

    float HeightAt(const int x, const int z) const
        { return textureData[z*XResolution() + x]; }

    void ChangeHeightAt(const int x, const int z, const float height)
        { textureData[z*XResolution() + x] = height; }

    void SyncVisualization()
        { heightMap.Update(textureData.data(), Texture2D::Red); }

    void Render(const alg::Mat4x4 &cameraMtx, const alg::Vec3 &camPos) const;

private:
    Texture2D heightMap;
    std::vector<float> textureData;

    alg::Vec3 mainHeightMapCorner;

    float xLen;
    float zLen;

    float initThickness;

    // Meshes
    Mesh top;
    Mesh bottom;
    Mesh sideX;
    Mesh sideZ;

    // Shaders
    MillingMaterialTopShader topShader;
    MillingMaterialSideShader sideShader;
    MillingMaterialBottomShader bottomShader;

    std::vector<float> GenerateMaterialTopVertices() const;
    std::vector<uint32_t> GenerateMaterialTopIndices() const;

    std::vector<float> GenerateMaterialBottomVertices() const;
    std::vector<uint32_t> GenerateMaterialBottomIndices() const;

    std::vector<float> GenerateMaterialSideXVertices() const;
    std::vector<uint32_t> GenerateMaterialSideXIndices() const;

    std::vector<float> GenerateMaterialSideZVertices() const;
    std::vector<uint32_t> GenerateMaterialSideZIndices() const;
};
