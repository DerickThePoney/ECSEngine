#pragma once

namespace ECSEngine
{
namespace Physics
{
bool OBBIntersection(const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB);
}
} // namespace ECSEngine