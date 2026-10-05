#include "stdafx.h"

#include "OBBCapsuleIntersection.h"

#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
bool OBBCapsuleIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3& parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB,
      const bool parInvertResult)
{
    // Capsule axis is the transform Y column (rigid body orientation, unit length).
    const vec3 AxisA = Normalize(parTransformA.Column(1).xyz());

    // Capsule segment endpoints in world space.
    const vec3 CenterAW = (parTransformA * vec4::MakeHomogeneousPositionVec4(parCenterA)).xyz();
    const vec3 AS = CenterAW - AxisA * parHalfLengthA;
    const vec3 AE = CenterAW + AxisA * parHalfLengthA;

    // Transform into OBB local space so the query is segment vs AABB.
    const mat4 InvTransformB = Invert(parTransformB);
    const vec3 ASLocal = (InvTransformB * vec4::MakeHomogeneousPositionVec4(AS)).xyz();
    const vec3 AELocal = (InvTransformB * vec4::MakeHomogeneousPositionVec4(AE)).xyz();

    const vec3 CenterBLocal = parBoundingBoxB.Center();
    const vec3 HalfExtents = parBoundingBoxB.HalfExtent();
    const vec3 MinB = CenterBLocal - HalfExtents;
    const vec3 MaxB = CenterBLocal + HalfExtents;

    GeometryHelpers::SegmentAABBClosestResult Closest;
    GeometryHelpers::ClosestPointsSegmentAABB(ASLocal, AELocal, MinB, MaxB, Closest);

    // Outside and beyond the capsule radius: no contact.
    if (!Closest.Interior && Closest.DistSq > parRadiusA * parRadiusA)
        return false;

    // Separating normal in box-local space: from the box toward the capsule (outward).
    vec3 SeparatingNormalLocal;
    if (Closest.Interior)
    {
        SeparatingNormalLocal = vec3(0.f);
        if (Closest.FaceAxis == 0)
            SeparatingNormalLocal.x = Closest.FaceSign;
        else if (Closest.FaceAxis == 1)
            SeparatingNormalLocal.y = Closest.FaceSign;
        else
            SeparatingNormalLocal.z = Closest.FaceSign;
    }
    else if (Closest.DistSq > 1e-6f)
    {
        SeparatingNormalLocal = (Closest.OnSegment - Closest.OnAABB) / sqrtf(Closest.DistSq);
    }
    else
    {
        SeparatingNormalLocal = vec3(0.f);
        if (Closest.FaceAxis == 0)
            SeparatingNormalLocal.x = Closest.FaceSign;
        else if (Closest.FaceAxis == 1)
            SeparatingNormalLocal.y = Closest.FaceSign;
        else
            SeparatingNormalLocal.z = Closest.FaceSign;
        if (LengthSq(SeparatingNormalLocal) <= 1e-6f)
            SeparatingNormalLocal = vec3(0.f, 1.f, 0.f);
    }

    // World-space normal from box toward capsule, then flip if the capsule is body A.
    vec3 NormalBoxToCap = (parTransformB * vec4::MakeHomogeneousDirectionVec4(SeparatingNormalLocal)).xyz();
    const float NormalLenSq = LengthSq(NormalBoxToCap);
    if (NormalLenSq > 1e-12f)
        NormalBoxToCap = NormalBoxToCap / sqrtf(NormalLenSq);
    else
        NormalBoxToCap = vec3(0.f, 1.f, 0.f);

    C->FContactNormal = parInvertResult ? Invert(NormalBoxToCap) : NormalBoxToCap;

    const vec3 OnSeg = (parTransformB * vec4::MakeHomogeneousPositionVec4(Closest.OnSegment)).xyz();
    const vec3 OnBox = (parTransformB * vec4::MakeHomogeneousPositionVec4(Closest.OnAABB)).xyz();

    // Single contact from the true closest feature only (no face-manifold expansion).
    // Expanding to a 2-point face manifold was regenerating phantom contacts across gaps.
    ContactPoint CP;
    if (parInvertResult)
    {
        // Capsule is body A: same convention as CapsuleSphereIntersection — offset along final normal.
        CP.FPosition = OnSeg + C->FContactNormal * parRadiusA;
    }
    else
    {
        // Box is body A: contact on the box surface (same as OBBSphereIntersection).
        CP.FPosition = OnBox;
    }

    // Match OBBIntersection: penetration is negative when overlapping so Baumgarte applies.
    if (Closest.Interior)
        CP.FPenetration = -(parRadiusA + Closest.ExitDepth);
    else
        CP.FPenetration = sqrtf(Closest.DistSq) - parRadiusA;

    C->FManifold.FContactPoints.push_back(CP);
    return true;
}
} // namespace Physics
} // namespace ECSEngine
