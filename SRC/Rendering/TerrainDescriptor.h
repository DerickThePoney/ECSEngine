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
};
} // namespace Rendering
} // namespace ECSEngine