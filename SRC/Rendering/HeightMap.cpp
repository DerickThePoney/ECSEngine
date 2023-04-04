#include "stdafx.h"

#include "HeightMap.h"

#include "Common/Resource.h"
#include "Common/ResourceHandle.h"
#include "Math/Vector.h"
#include "RenderingCore/Texture.h"
#include "RenderingCore/TextureDescriptor.h"
#include "RenderingCore/TexturesManager.h"
#include "bimg/bimg.h "

namespace ECSEngine
{
namespace Rendering
{

HeightMap::HeightMap()
{
}

HeightMap::~HeightMap()
{
}

void HeightMap::Initialise(const TerrainDescriptor& parTerrainDescriptor, const TextureHandle& parHeightMapTexture)
{
    FTerrainDescriptor = parTerrainDescriptor;
    ReadHeigthMapTexture(parHeightMapTexture);
}

void HeightMap::Shutdown()
{
}

vec2 HeightMap::GetMinMaxHeightInBBox(const BoundingBox<vec3>& parBBox)
{
    // find coordinates
    vec2 minMaxLine(parBBox.Min().x, parBBox.Max().x);
    minMaxLine = minMaxLine / FTerrainDescriptor.TerrainSize * FHeightMapSize.xx();
    vec2 minMaxColumn = vec2(parBBox.Min().z, parBBox.Max().z);
    minMaxColumn = minMaxColumn / FTerrainDescriptor.TerrainSize * FHeightMapSize.yy();

    vec2 res(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (i32 line = minMaxLine.x; line < minMaxLine.y; ++line)
    {
        for (i32 column = minMaxColumn.x; column < minMaxColumn.y; ++column)
        {
            const u32 idx = line * FHeightMapSize.y + column;
            const float value = FHeightMapData[idx];
            if (value < res.x)
                res.x = value;
            if (value > res.y)
                res.y = value;
        }
    }

    return res;
}

void HeightMap::ReadHeigthMapTexture(const TextureHandle& parHeightMapTexture)
{
    const Texture* heightMapTexture = TextureManager::Instance().GetTexture(parHeightMapTexture);
    AssertRelease(heightMapTexture != nullptr);
    const TextureDescriptor* heightMapTextureDescriptor = heightMapTexture->Descriptor();
    AssertRelease(heightMapTextureDescriptor != nullptr);

    const std::string& heightMapFile = heightMapTextureDescriptor->TextureFile();
    std::size_t found = heightMapFile.find_last_of('\\');
    AssertRelease(found != heightMapFile.npos);
    Resource hmResource(heightMapFile.substr(0, found + 1) + heightMapTexture->TextureName() + ".ktx");

    std::shared_ptr<ResourceHandle> heightMapResourceHandle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&hmResource);
    AssertRelease(heightMapResourceHandle != nullptr);

    bimg::ImageContainer heightMapContainer;
    const bool result = bimg::imageParse(heightMapContainer, heightMapResourceHandle->Buffer(), heightMapResourceHandle->Size());
    AlwaysCheckedAssert(result);

    if (result)
    {
        bimg::ImageMip mip;
        const bool result2 = bimg::imageGetRawData(heightMapContainer, 0, 0, heightMapResourceHandle->Buffer(), heightMapResourceHandle->Size(), mip);
        AssertRelease(result2);

        FHeightMapData.resize(mip.m_height * mip.m_width);
        FHeightMapSize = vec2(mip.m_height, mip.m_width);
        const u32* mipData = reinterpret_cast<const u32*>(mip.m_data);

        for (i32 line = 0; line < mip.m_height; ++line)
        {
            for (i32 column = 0; column < mip.m_width; ++column)
            {
                const u32 idxMap = line * mip.m_width + column;
                const u32 idxMip = column * mip.m_height + line;
                const u32 value = mipData[idxMip] & 0xFF;
                const float valuef = (float)value / 255.f;
                FHeightMapData[idxMap] = valuef * 50.f;
            }
        }
    }
}

} // namespace Rendering
} // namespace ECSEngine
