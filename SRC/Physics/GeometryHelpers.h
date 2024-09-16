#pragma once
#include "Common/BoundingBox.h"

namespace ECSEngine
{
namespace Physics
{
namespace GeometryHelpers
{
AABB3f ComputeAABBFromOBB(const AABB3f& OBB, const mat4& Transform);

bool FirstAABBContainsSecond(const AABB3f& parFirst, const AABB3f& parSecond);
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine