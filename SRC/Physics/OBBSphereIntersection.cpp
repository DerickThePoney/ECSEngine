#include "stdafx.h"

#include "OBBSphereIntersection.h"

#include "Contact.h"

namespace ECSEngine
{
namespace Physics
{
bool OBBSphereIntersection(Contact* C,
      const mat4& parTransformA,
      const AABB3f& parBoundingBoxA,
      const mat4& parTransformB,
      const vec4& parCenter,
      const float parRadius,
      const bool bNeedResultInSphereFrame)
{
    // Move the center of the sphere in the OBB frame
    const vec4 SphereInOBBFrame = Invert(parTransformA) * parTransformB * parCenter;

    // Distance between OBB and sphere
    const vec3 DistanceOBBSPhere = SphereInOBBFrame.xyz();

    // Clamping distance on the bounding box extents to get closest points
    vec3 ClosestPoint = DistanceOBBSPhere;
    const vec3 HalfExtents = parBoundingBoxA.HalfExtent();
    ClosestPoint.x = Clamp(ClosestPoint.x, -HalfExtents.x, HalfExtents.x);
    ClosestPoint.y = Clamp(ClosestPoint.y, -HalfExtents.y, HalfExtents.y);
    ClosestPoint.z = Clamp(ClosestPoint.z, -HalfExtents.z, HalfExtents.z);

    // Compute the distance to the sphere of the closest point
    const vec3 DistanceToSphere = DistanceOBBSPhere - ClosestPoint;
    const float DistSQ = Dot(DistanceToSphere, DistanceToSphere);

    if (DistSQ > parRadius * parRadius)
    {
        return false;
    }

    // Fill up the contact
    const float Dist = sqrtf(DistSQ);
    C->FContactNormal = (Dist > 1e-6f) ? DistanceToSphere / Dist : vec3(1.f, 0.f, 0.f);
    if (bNeedResultInSphereFrame)
    {
        C->FContactNormal = Invert(C->FContactNormal);
    }
    C->FContactNormal = (parTransformA * vec4::MakeHomogeneousDirectionVec4(C->FContactNormal)).xyz();

    ContactPoint CP;
    CP.FPosition = (parTransformA * vec4::MakeHomogeneousPositionVec4(ClosestPoint)).xyz();
    CP.FPenetration = parRadius - Dist;
    C->FManifold.FContactPoints.push_back(CP);

    return true;
}
} // namespace Physics
} // namespace ECSEngine