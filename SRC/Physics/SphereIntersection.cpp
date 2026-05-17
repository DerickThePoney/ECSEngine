#include "stdafx.h"

#include "SphereIntersection.h"

#include "Contact.h"

namespace ECSEngine
{
namespace Physics
{
bool SphereIntersection(Contact* C,
      const mat4& parTransformA,
      const vec4& parCenterA,
      const float parRadiusA,
      const mat4& parTransformB,
      const vec4& parCenterB,
      const float parRadiusB)
{
    const vec4 CenterAW = parTransformA * parCenterA;
    const vec4 CenterBW = parTransformB * parCenterB;

    const vec4 DistAB = CenterBW - CenterAW;
    const float TouchingDistance = parRadiusA + parRadiusB;
    const float TouchingDistanceSq = TouchingDistance * TouchingDistance;

    const float DistABLengthSQ = LengthSq(DistAB.xyz());
    if (DistABLengthSQ > TouchingDistanceSq)
    {
        return false;
    }

    C->FContactNormal = (DistABLengthSQ > 1e-6f) ? Normalize(DistAB.xyz()) : vec3(1.f, 0.f, 0.f);
    ContactPoint CP;
    CP.FPosition = CenterAW.xyz() + C->FContactNormal * parRadiusA;
    CP.FPenetration = TouchingDistance - sqrtf(DistABLengthSQ);
    C->FManifold.FContactPoints.push_back(CP);
    return true;
}
} // namespace Physics
} // namespace ECSEngine