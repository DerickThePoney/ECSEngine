#pragma once
#include "Common/RenderingHandles.h"
#include "TerrainDescriptor.h"
#include "TerrainQuadTree.h"

namespace ECSEngine
{
namespace Rendering
{
class TerrainRenderer
{
public:
    TerrainRenderer();
    ~TerrainRenderer();

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