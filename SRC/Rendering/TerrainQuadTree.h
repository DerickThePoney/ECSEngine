#pragma once
#include "Common/BoundingBox.h"

namespace ECSEngine
{
class Camera;
namespace Rendering
{
struct TerrainDescriptor;

struct QuadTreeNode
{
    u32 ChildrenIdx[4] = { -1u, -1u, -1u, -1u };
    BoundingBox<vec3> BBox;
    u32 LoDLevel = 0;
    u32 NodeIdx = -1;
};

class TerrainQuadTree
{
public:
    void Initialize(const TerrainDescriptor* parDescriptor);
    void FillRegionsToRender(const Camera& parCamera, std::vector<QuadTreeNode>& outNodesToRender);

private:
    const TerrainDescriptor* FDescriptor = nullptr;
    std::vector<QuadTreeNode> FNodes;
};
} // namespace Rendering
} // namespace ECSEngine