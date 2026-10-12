#include "stdafx.h"

#include "RayCapsuleIntersection.h"

#include "RaySphereIntersection.h"
#include "Raycast.h"

namespace ECSEngine
{
namespace Physics
{
namespace
{
// Keep the closer of two hits (Distance is t along a unit ray).
void KeepClosestHit(bool bCandidateValid, const RaycastHit& Candidate, bool& bBestValid, RaycastHit& Best)
{
    if (!bCandidateValid)
        return;

    if (!bBestValid || Candidate.Distance < Best.Distance)
    {
        Best = Candidate;
        bBestValid = true;
    }
}

// Finite cylinder wall: axis segment [A, B], radius R.
// Same idea as before: solve |perp| = R, then keep only hits with axial s in [0, L].
bool RayFiniteCylinderIntersection(const Ray3D& Ray, float MaxDistance, const vec3& A, const vec3& B, float Radius, RaycastHit& OutHit)
{
    const vec3 AB = B - A;
    const float L = Length(AB);
    if (L < 1e-8f)
        return false;

    const vec3 U = AB / L; // unit axis

    // Offset from A to ray origin, and ray direction, split into // and ⊥ to U.
    const vec3 W = Ray.FOrigin - A;
    const vec3 DPerp = Ray.FDirection - U * Dot(Ray.FDirection, U);
    const vec3 WPerp = W - U * Dot(W, U);

    const float ACoeff = Dot(DPerp, DPerp);

    // Ray parallel to axis: never enters the side wall from the side.
    // (Ends are handled by the sphere caps.)
    if (ACoeff < 1e-12f)
        return false;

    // |WPerp + t DPerp|^2 = R^2
    const float BCoeff = 2.f * Dot(WPerp, DPerp);
    const float CCoeff = Dot(WPerp, WPerp) - Radius * Radius;
    const float Disc = BCoeff * BCoeff - 4.f * ACoeff * CCoeff;
    if (Disc < 0.f)
        return false;

    const float Inv2A = 0.5f / ACoeff;
    const float SqrtDisc = sqrtf(Disc);
    const float T0 = (-BCoeff - SqrtDisc) * Inv2A;
    const float T1 = (-BCoeff + SqrtDisc) * Inv2A;

    bool bFound = false;
    float BestT = MaxDistance;

    // Test both roots; keep the nearest valid entry on the finite segment.
    const float Candidates[2] = { T0, T1 };
    for (float T : Candidates)
    {
        if (T < 0.f || T >= MaxDistance || T >= BestT)
            continue;

        const vec3 HitPoint = Ray.FOrigin + Ray.FDirection * T;

        // How far along the axis from A? Must lie on [0, L].
        const float S = Dot(HitPoint - A, U);
        if (S < 0.f || S > L)
            continue;

        // Foot on axis; radial direction is the outward normal.
        const vec3 AxisFoot = A + U * S;
        vec3 Normal = HitPoint - AxisFoot;
        const float NormalLenSq = Dot(Normal, Normal);
        if (NormalLenSq < 1e-12f)
            continue; // numerically on the axis

        BestT = T;
        OutHit.Distance = T;
        OutHit.Point = HitPoint;
        OutHit.Normal = Normal * (1.f / sqrtf(NormalLenSq));
        bFound = true;
    }

    return bFound;
}

// True if ray origin is inside the capsule (distance to segment <= radius).
bool IsOriginInsideCapsule(const vec3& Origin, const vec3& A, const vec3& B, float Radius)
{
    const vec3 AB = B - A;
    const float L2 = Dot(AB, AB);
    float T = 0.f;
    if (L2 > 1e-12f)
        T = Clamp01(Dot(Origin - A, AB) / L2);

    const vec3 Closest = A + AB * T;
    const vec3 Delta = Origin - Closest;
    return Dot(Delta, Delta) <= Radius * Radius;
}
} // namespace

bool RayCapsuleIntersection(const Ray3D& parRay,
      float parMaxDistance,
      const mat4& parTransformC,
      const vec3 parCenterC,
      const float parRadiusC,
      const float parHalfLengthC,
      RaycastHit& outHit)
{
    // Local capsule is Y-aligned around Center: segment ± HalfLength on Y, then body transform.
    // Matches CollisionShape::ComputeAABB for CAPSULE.
    const vec3 LocalA = parCenterC + vec3(0.f, -parHalfLengthC, 0.f);
    const vec3 LocalB = parCenterC + vec3(0.f, +parHalfLengthC, 0.f);
    const vec3 A = (parTransformC * vec4::MakeHomogeneousPositionVec4(LocalA)).xyz();
    const vec3 B = (parTransformC * vec4::MakeHomogeneousPositionVec4(LocalB)).xyz();

    // Degenerate capsule → sphere at center.
    if (parHalfLengthC <= 0.f)
    {
        return RaySphereIntersection(parRay, parMaxDistance, parTransformC, parCenterC, parRadiusC, outHit);
    }

    // Same policy as RaySphereIntersection: origin inside counts as a hit at t = 0.
    if (IsOriginInsideCapsule(parRay.FOrigin, A, B, parRadiusC))
    {
        outHit.Distance = 0.f;
        outHit.Normal = Invert(parRay.FDirection);
        outHit.Point = parRay.FOrigin;
        return true;
    }

    bool bBestValid = false;
    RaycastHit BestHit;

    // 1) Side wall (finite cylinder between A and B).
    {
        RaycastHit CylinderHit;
        const bool bHit = RayFiniteCylinderIntersection(parRay, parMaxDistance, A, B, parRadiusC, CylinderHit);
        KeepClosestHit(bHit, CylinderHit, bBestValid, BestHit);
    }

    // 2) End caps — reuse sphere raycast at the world endpoints.
    //    Identity transform + world center so we don't double-apply parTransformC.
    {
        RaycastHit CapHit;
        const bool bHitA = RaySphereIntersection(parRay, parMaxDistance, mat4::Identity(), A, parRadiusC, CapHit);
        KeepClosestHit(bHitA, CapHit, bBestValid, BestHit);

        const bool bHitB = RaySphereIntersection(parRay, parMaxDistance, mat4::Identity(), B, parRadiusC, CapHit);
        KeepClosestHit(bHitB, CapHit, bBestValid, BestHit);
    }

    if (!bBestValid)
        return false;

    outHit = BestHit;
    return true;
}
} // namespace Physics
} // namespace ECSEngine