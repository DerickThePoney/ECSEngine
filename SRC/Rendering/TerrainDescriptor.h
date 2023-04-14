#pragma once
namespace ECSEngine
{
namespace Rendering
{
struct TerrainDescriptor
{
    u32 MeshVerticesSize = 64;
    float TerrainSize = 1000.f;
    u32 NumberLoDLevels = 10;
    float MinLodDistance = 15.f;
    std::vector<float> LoDDistances;

    SERIALIZE()
    {
        NAMEDPROPERTYFIELD("MeshVerticesSize", MeshVerticesSize, 64);
        NAMEDPROPERTYFIELD("TerrainSize", TerrainSize, 1000.f);
        NAMEDPROPERTYFIELD("NumberLoDLevels", NumberLoDLevels, 10);
        NAMEDPROPERTYFIELD("MinLodDistance", MinLodDistance, 15.f);
    }
};
} // namespace Rendering
} // namespace ECSEngine