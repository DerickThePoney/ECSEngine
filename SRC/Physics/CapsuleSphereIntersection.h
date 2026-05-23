#pragma once

namespace ECSEngine
{
namespace Physics
{
struct Contact;
bool CapsuleSphereIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3 parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const vec3 parCenterB,
      const float parRadiusB,
      const bool parInvertResult);
} // namespace Physics
} // namespace ECSEngine