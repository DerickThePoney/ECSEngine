#pragma once
namespace ECSEngine
{
namespace Physics
{
struct Contact;
bool SphereIntersection(Contact* C,
      const mat4& parTransformA,
      const vec4& parCenterA,
      const float parRadiusA,
      const mat4& parTransformB,
      const vec4& parCenterB,
      const float parRadiusB);
} // namespace Physics
} // namespace ECSEngine