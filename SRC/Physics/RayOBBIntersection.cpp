#include "stdafx.h"

#include "RayOBBIntersection.h"

#include "Common/IntersectionRoutines.h"
#include "Math/MatrixUtils.h"
#include "Raycast.h"

namespace ECSEngine
{
namespace Physics
{
// local helpers (or share with OBBCapsuleIntersection)
float Component(const vec3& V, i32 Axis)
{
    switch (Axis)
    {
    case 0:
        return V.x;
    case 1:
        return V.y;
    default:
        return V.z;
    }
}

void SetComponent(vec3& V, i32 Axis, float Value)
{
    switch (Axis)
    {
    case 0:
        V.x = Value;
        break;
    case 1:
        V.y = Value;
        break;
    default:
        V.z = Value;
        break;
    }
}

bool RayOBBIntersection(const Ray3D& parRay, float parMaxDistance, const mat4& parTransformB, const AABB3f& parBoundingBoxB, RaycastHit& outHit)
{
    const mat4 InvTransform = Invert(parTransformB);

    Ray3D LocalRay;
    LocalRay.FOrigin = (InvTransform * vec4::MakeHomogeneousPositionVec4(parRay.FOrigin)).xyz();
    LocalRay.FDirection = (InvTransform * vec4::MakeHomogeneousDirectionVec4(parRay.FDirection)).xyz();

    float TMin = 0.f;
    float TMax = 0.f;
    i32 HitAxis = -1;
    float HitSign = 0.f;

    if (!Intersection::RayAABBIntersection(LocalRay, parBoundingBoxB, parMaxDistance, TMin, TMax, HitAxis, HitSign))
        return false;

    if (TMin <= 0.f)
    {
        outHit.Distance = 0.f;
        outHit.Point = parRay.FOrigin;
        outHit.Normal = Invert(parRay.FDirection);
        return true;
    }

    outHit.Distance = TMin;
    outHit.Point = parRay.FOrigin + parRay.FDirection * TMin;

    vec3 NormalLocal(0.f);
    if (HitAxis >= 0)
        SetComponent(NormalLocal, HitAxis, HitSign);
    else
        NormalLocal = vec3(0.f, 1.f, 0.f);

    outHit.Normal = Normalize((parTransformB * vec4::MakeHomogeneousDirectionVec4(NormalLocal)).xyz());
    if (Dot(outHit.Normal, parRay.FDirection) > 0.f)
        outHit.Normal = Invert(outHit.Normal);

    return true;
}
} // namespace Physics
} // namespace ECSEngine