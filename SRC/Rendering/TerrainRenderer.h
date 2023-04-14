#pragma once
#include "Common/RenderingHandles.h"
#include "Common/Singleton.h"
#include "TerrainDescriptor.h"
#include "TerrainQuadTree.h"

namespace ECSEngine
{
namespace Rendering
{
class TerrainRenderer : public Singleton<TerrainRenderer>
{
public:
    TerrainRenderer();
    ~TerrainRenderer();

    void Initialize(const TerrainDescriptor& parTerrainDescriptor, bool parIsEditor = false);
    void Reinitialize(const TerrainDescriptor& parTerrainDescriptor, bool parIsEditor = false);
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