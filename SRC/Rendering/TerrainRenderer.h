#pragma once
#include "Common/RenderingHandles.h"
#include "TerrainQuadTree.h"

namespace ECSEngine
{
namespace Rendering
{
struct TerrainDescriptor
{
    u32 MeshVerticesSize = 16;
    float TerrainSize = 1000.f;
    u32 NumberLoDLevels = 10;
    float MinLodDistance = 15.f;
    std::vector<float> LoDDistances;
};

class TerrainRenderer
{
public:
    void Initialize();
    void Shutdown();

    void Render();

private:
    void CreateTerrainMesh();

private:
    DrawCommandBuffer* CommandBuffer = nullptr;

    TerrainQuadTree FQuadTree;
    TerrainDescriptor FTerrainDescriptor;
    MeshHandle FTerrainMesh;
};
} // namespace Rendering
} // namespace ECSEngine