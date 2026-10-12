#pragma once
#include "Common/Ray.h"

namespace ECSEngine
{
namespace Physics
{
struct RaycastHit;

bool RaySphereIntersection(const Ray3D& parRay, float parMaxDistance, const mat4& parTransformS, const vec3 parCenterS, const float parRadiusS, RaycastHit& outHit);
} // namespace Physics
} // namespace ECSEngine