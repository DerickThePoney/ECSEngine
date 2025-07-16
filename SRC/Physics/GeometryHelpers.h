#pragma once
#include "Common/BoundingBox.h"
#include "Common/MemoryView.h"

namespace ECSEngine
{
namespace Physics
{
namespace GeometryHelpers
{
void ExtractAABBCorners(const AABB3f& OBB, MemoryView<vec4>& parView);
void ExtractOBBCorners(const AABB3f& OBB, const mat4& Transform, MemoryView<vec4>& parView);
float ProjectBoxToAxis(const vec4& parAxis, const vec4& parCenter, MemoryView<vec4>& parView);
float ProjectBoxToAxis(const vec4& parAxis, const mat4& parTransform, const AABB3f& parBBox);
AABB3f ComputeAABBFromOBB(const AABB3f& OBB, const mat4& Transform);

bool FirstAABBContainsSecond(const AABB3f& parFirst, const AABB3f& parSecond);
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine