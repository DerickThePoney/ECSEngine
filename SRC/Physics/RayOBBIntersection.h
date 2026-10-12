#pragma once
#include "Common/Ray.h"

namespace ECSEngine
{
namespace Physics
{
struct RaycastHit;

bool RayOBBIntersection(const Ray3D& parRay, float parMaxDistance, const mat4& parTransformB, const AABB3f& parBoundingBoxB, RaycastHit& outHit);
} // namespace Physics
} // namespace ECSEngine