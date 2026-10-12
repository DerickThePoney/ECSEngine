#include "stdafx.h"

#include "RaySphereIntersection.h"

#include "Raycast.h"

namespace ECSEngine
{
namespace Physics
{
bool RaySphereIntersection(const Ray3D& parRay, float parMaxDistance, const mat4& parTransformS, const vec3 parCenterS, const float parRadiusS, RaycastHit& outHit)
{
    vec3 SphereCenterW = (parTransformS * vec4::MakeHomogeneousPositionVec4(parCenterS)).xyz();

    vec3 m = parRay.FOrigin - SphereCenterW;

    float b = Dot(m, parRay.FDirection);
    float c = Dot(m, m) - parRadiusS * parRadiusS;

    if (c < 0)
    {
        // inside...
        outHit.Distance = 0.f;
        outHit.Normal = Invert(parRay.FDirection);
        outHit.Point = parRay.FOrigin;
        return true;
    }

    float disc = b * b - c;
    if (disc < 0)
        return false;

    float t = -b - sqrtf(disc);
    if (t < 0 || t >= parMaxDistance)
        return false;

    outHit.Distance = t;
    outHit.Point = parRay.FOrigin + t * parRay.FDirection;
    outHit.Normal = Normalize(outHit.Point - SphereCenterW);

    return true;
}
} // namespace Physics
} // namespace ECSEngine