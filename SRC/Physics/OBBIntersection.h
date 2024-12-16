#pragma once

namespace ECSEngine
{
namespace Physics
{
struct Contact;
bool OBBIntersection(Contact* C, const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB);
} // namespace Physics
} // namespace ECSEngine