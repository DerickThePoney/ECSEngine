#pragma once

namespace ECSEngine
{
namespace Physics
{
struct Contact;
bool CapsuleIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3 parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const vec3 parCenterB,
      const float parRadiusB,
      const float parHalfLengthB);
} // namespace Physics
} // namespace ECSEngine