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

    [[nodiscard]]
    float PixelXLen() const
        { return xLen / static_cast<float>(XResolution()); }

    [[nodiscard]]
    float PixelZLen() const
        { return zLen / static_cast<float>(ZResolution()); }

    [[nodiscard]]
    float MinX() const
        { return mainHeightMapCorner.X(); }

    [[nodiscard]]
    float MinZ() const
        { return mainHeightMapCorner.Z(); }

    [[nodiscard]]
    float GlobalX(int x) const;

    [[nodiscard]]
    float GlobalZ(int z) const;

    [[nodiscard]]
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

    void UpdateMeshes();

    [[nodiscard]] std::vector<float> GenerateMaterialTopVertices() const;
    [[nodiscard]] std::vector<uint32_t> GenerateMaterialTopIndices() const;

    [[nodiscard]] std::vector<float> GenerateMaterialBottomVertices() const;
    [[nodiscard]] std::vector<uint32_t> GenerateMaterialBottomIndices() const;

    [[nodiscard]] std::vector<float> GenerateMaterialSideXVertices() const;
    [[nodiscard]] std::vector<uint32_t> GenerateMaterialSideXIndices() const;

    [[nodiscard]] std::vector<float> GenerateMaterialSideZVertices() const;
    [[nodiscard]] std::vector<uint32_t> GenerateMaterialSideZIndices() const;
};
