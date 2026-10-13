#pragma once
#include "Common/Ray.h"

namespace ECSEngine
{
namespace Physics
{
struct RaycastHit;

bool RayCapsuleIntersection(const Ray3D& parRay,
      float parMaxDistance,
      const mat4& parTransformC,
      const vec3 parCenterC,
      const float parRadiusC,
      const float parHalfLengthC,
      RaycastHit& outHit);
} // namespace Physics
} // namespace ECSEngine