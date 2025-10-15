#include <CAD_modeler/model/components/millingMaterial.hpp>

#include <CAD_modeler/model/systems/shaders/shaderRepository.hpp>

#include <algorithm>
#include <numeric>


MillingMaterial::MillingMaterial(const int xResolution, const int zResolution, const float xLen, const float zLen, const float thickness):
    heightMap(xLen, zLen, nullptr, Texture2D::Red32BitFloat, Texture2D::Red),
    mainHeightMapCorner(-xLen/2.f, 0.f, -zLen/2.f), xLen(xLen), zLen(zLen), initThickness(thickness)
{
    textureData.resize(xResolution * zResolution);
    std::ranges::fill(textureData, initThickness);

    top.Update(
        GenerateMaterialTopVertices(),
        GenerateMaterialTopIndices()
    );

    bottom.Update(
        GenerateMaterialBottomVertices(),
        GenerateMaterialBottomIndices()
    );

    sideX.Update(
        GenerateMaterialSideXVertices(),
        GenerateMaterialSideXIndices()
    );

    sideZ.Update(
        GenerateMaterialSideZVertices(),
        GenerateMaterialSideZIndices()
    );
}


void MillingMaterial::SetResolution(const int xRes, const int zRes)
{
    textureData.resize(xRes * zRes);
    std::ranges::fill(textureData, initThickness);
    heightMap.ChangeSize(xRes, zRes, textureData.data(), Texture2D::Red);

    top.Update(
        GenerateMaterialTopVertices(),
        GenerateMaterialTopIndices()
    );

    sideX.Update(
         GenerateMaterialSideXVertices(),
         GenerateMaterialSideXIndices()
    );

    sideZ.Update(
        GenerateMaterialSideZVertices(),
        GenerateMaterialSideZIndices()
    );
}


void MillingMaterial::SetSize(const float xLen, const float zLen)
{
    this->xLen = xLen;
    this->zLen = zLen;

    mainHeightMapCorner.X() = -xLen / 2.f;
    mainHeightMapCorner.Z() = -zLen / 2.f;

    top.Update(
        GenerateMaterialTopVertices(),
        GenerateMaterialTopIndices()
    );

    bottom.Update(
        GenerateMaterialBottomVertices(),
        GenerateMaterialBottomIndices()
    );

    sideX.Update(
        GenerateMaterialSideXVertices(),
        GenerateMaterialSideXIndices()
    );

    sideZ.Update(
        GenerateMaterialSideZVertices(),
        GenerateMaterialSideZIndices()
    );
}


void MillingMaterial::SetThickness(const float thickness)
{
    initThickness = thickness;
    Reset();
}


void MillingMaterial::SetBaseLevel(const float level)
{
    mainHeightMapCorner.Y() = level;

    top.Update(
        GenerateMaterialTopVertices(),
        GenerateMaterialTopIndices()
    );

    bottom.Update(
        GenerateMaterialBottomVertices(),
        GenerateMaterialBottomIndices()
    );

    sideX.Update(
        GenerateMaterialSideXVertices(),
        GenerateMaterialSideXIndices()
     );

    sideZ.Update(
        GenerateMaterialSideZVertices(),
        GenerateMaterialSideZIndices()
    );
}


void MillingMaterial::Reset()
{
    std::ranges::fill(textureData, initThickness);

    heightMap.Update(textureData.data(), Texture2D::Red);
}


float MillingMaterial::GlobalX(const int x) const
{
    const float pixelXLen = PixelXLen();
    return mainHeightMapCorner.X() + pixelXLen/2.f + pixelXLen * x;
}


float MillingMaterial::GlobalZ(const int z) const
{
    const float pixelZLen = PixelZLen();
    return mainHeightMapCorner.Z() + pixelZLen/2.f + pixelZLen * z;
}


void MillingMaterial::Render(const alg::Mat4x4 &cameraMtx, const alg::Vec3 &camPos) const
{
    const auto& shaderRepo = ShaderRepository::GetInstance();
    const auto& materialTopShader = shaderRepo.GetMillingMaterialTopShader();

    // Render material top
    materialTopShader.Use();
    heightMap.Use();
    materialTopShader.SetCameraPosition(camPos);

    materialTopShader.SetHeightMapZLen(zLen);
    materialTopShader.SetHeightMapXLen(xLen);
    materialTopShader.SetMainHeightmapCorner(mainHeightMapCorner);
    materialTopShader.SetMVP(cameraMtx);

    top.Use();
    glDrawElements(GL_TRIANGLES, top.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    // Render material bottom
    const auto& materialRestShader = shaderRepo.GetMillingMaterialBottomShader();

    materialRestShader.Use();

    materialRestShader.SetCameraPosition(camPos);
    materialRestShader.SetNormal(-alg::Vec3::UnitY());
    materialRestShader.SetVP(cameraMtx);

    bottom.Use();
    glDrawElements(GL_TRIANGLE_STRIP, bottom.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    // Render sides
    const auto& materialSideShader = shaderRepo.GetMillingMaterialSideShader();

    materialSideShader.Use();
    heightMap.Use();
    materialSideShader.SetCameraPosition(camPos);
    materialSideShader.SetHeightMapZLen(zLen);
    materialSideShader.SetHeightMapXLen(xLen);
    materialSideShader.SetMainHeightmapCorner(mainHeightMapCorner);
    materialSideShader.SetVP(cameraMtx);

    // Positive X side
    materialSideShader.SetNormal(alg::Vec3::UnitX());
    materialSideShader.SetSideType(MillingMaterialSideShader::SideType::PositiveX);

    sideX.Use();
    glDrawElements(GL_TRIANGLE_STRIP, sideX.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    // Negative X side
    materialSideShader.SetNormal(-alg::Vec3::UnitX());
    materialSideShader.SetSideType(MillingMaterialSideShader::SideType::NegativeX);
    glDrawElements(GL_TRIANGLE_STRIP, sideX.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    // Positive Z side
    materialSideShader.SetNormal(alg::Vec3::UnitZ());
    materialSideShader.SetSideType(MillingMaterialSideShader::SideType::PositiveZ);

    sideZ.Use();
    glDrawElements(GL_TRIANGLE_STRIP, sideZ.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);

    // Negative Z side
    materialSideShader.SetNormal(-alg::Vec3::UnitZ());
    materialSideShader.SetSideType(MillingMaterialSideShader::SideType::NegativeZ);
    glDrawElements(GL_TRIANGLE_STRIP, sideZ.GetElementsCnt(), GL_UNSIGNED_INT, nullptr);
}


std::vector<float> MillingMaterial::GenerateMaterialTopVertices() const
{
    std::vector<float> result;
    const int pointsInX = XResolution();
    const int pointsInZ = ZResolution();

    result.reserve(pointsInX * pointsInZ * alg::Vec3::dim);

    const float pixelXLen = PixelXLen();
    const float pixelZLen = PixelZLen();

    for (int row = 0; row < pointsInZ; row++) {
        for (int col = 0; col < pointsInX; col++) {
            const float x = pixelXLen/2.f + col * pixelXLen;
            const float z = pixelZLen/2.f + row * pixelZLen;

            result.push_back(mainHeightMapCorner.X() + x);
            result.push_back(mainHeightMapCorner.Y());
            result.push_back(mainHeightMapCorner.Z() + z);
        }
    }

    return result;
}


std::vector<uint32_t> MillingMaterial::GenerateMaterialTopIndices() const
{
    std::vector<uint32_t> result;

    const int pointsInX = XResolution();
    const int pointsInZ = ZResolution();

    result.reserve((pointsInZ - 1) * (pointsInX - 1) * 6);

    for (int row = 0; row < pointsInZ - 1; row ++) {
        for (int col = 0; col < pointsInX - 1; col++) {

            // First triangle
            result.push_back(col + pointsInX * row);
            result.push_back(col + pointsInX * row + 1);
            result.push_back(col + pointsInX * row + pointsInX + 1);

            // Second triangle
            result.push_back(col + pointsInX * row);
            result.push_back(col + pointsInX * row + pointsInX + 1);
            result.push_back(col + pointsInX * row + pointsInX);
        }
    }

    return result;
}


std::vector<float> MillingMaterial::GenerateMaterialBottomVertices() const
{
    const float valX = xLen / 2.f;
    const float valY = BaseLevel();
    const float valZ = zLen / 2.f;

    return std::vector{
        // First vertex
        -valX,
         valY,
        -valZ,

        // Second vertex
         valX,
         valY,
        -valZ,

        // Third vertex
        -valX,
         valY,
         valZ,

        // Forth vertex
        valX,
        valY,
        valZ
    };
}


std::vector<uint32_t> MillingMaterial::GenerateMaterialBottomIndices() const
{
    return std::vector{
        0u, 1u, 2u, 3u
    };
}


std::vector<float> MillingMaterial::GenerateMaterialSideXVertices() const
{
    std::vector<float> result;
    result.reserve(ZResolution() * 2 * alg::Vec3::dim);

    const float baseLevel = BaseLevel();

    for (int z = 0; z < ZResolution(); z++) {
        constexpr float posX = 0.f;
        const float posY = baseLevel;
        const float posZ = GlobalZ(z);

        result.push_back(posX);
        result.push_back(posY);
        result.push_back(posZ);

        result.push_back(posX);
        result.push_back(posY);
        result.push_back(posZ);
    }

    return result;
}


std::vector<uint32_t> MillingMaterial::GenerateMaterialSideXIndices() const
{
    std::vector<uint32_t> result(ZResolution() * 2);

    std::iota(result.begin(), result.end(), 0u);

    return result;
}


std::vector<float> MillingMaterial::GenerateMaterialSideZVertices() const
{
    std::vector<float> result;
    result.reserve(XResolution() * 2 * alg::Vec3::dim);

    const float baseLevel = BaseLevel();

    for (int x = 0; x < XResolution(); x++) {
        const float posX = GlobalX(x);
        const float posY = baseLevel;
        constexpr float posZ = 0.f;

        result.push_back(posX);
        result.push_back(posY);
        result.push_back(posZ);

        result.push_back(posX);
        result.push_back(posY);
        result.push_back(posZ);
    }

    return result;
}


std::vector<uint32_t> MillingMaterial::GenerateMaterialSideZIndices() const
{
    std::vector<uint32_t> result(XResolution() * 2);

    std::iota(result.begin(), result.end(), 0u);

    return result;
}
