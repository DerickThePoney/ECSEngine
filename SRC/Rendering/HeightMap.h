#pragma once
#include "Common/BoundingBox.h"
#include "TerrainDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
class TextureHandle;
class HeightMap : public Singleton<HeightMap>
{
public:
    HeightMap();
    ~HeightMap();

    void Initialise(const TerrainDescriptor& parTerrainDescriptor, const TextureHandle& parHeightMapTexture);
    void Shutdown();

    vec2 GetMinMaxHeightInBBox(const BoundingBox<vec3>& parBBox);

private:
    void ReadHeigthMapTexture(const TextureHandle& parHeightMapTexture);

private:
    TerrainDescriptor FTerrainDescriptor;
    std::vector<float> FHeightMapData;
    vec2 FHeightMapSize;
};

} // namespace Rendering
} // namespace ECSEngine
