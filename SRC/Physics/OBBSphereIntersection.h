#pragma once

namespace ECSEngine
{
namespace Physics
{
struct Contact;
bool OBBSphereIntersection(Contact* C,
      const mat4& parTransformA,
      const AABB3f& parBoundingBoxA,
      const mat4& parTransformB,
      const vec4& parCenter,
      const float parRadius,
      const bool bNeedResultInSphereFrame);
} // namespace Physics
} // namespace ECSEngine