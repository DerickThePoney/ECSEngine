#include "stdafx.h"

#include "OBBCapsuleIntersection.h"

#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
namespace
{
void EmitContact(Contact* C, const mat4& parTransformB, const vec3& parOnSegLocal, const vec3& parOnBoxLocal, const vec3& parCapsuleOffsetDir, const float parRadiusA,
      const float parPenetration, const bool parInvertResult)
{
    const vec3 OnSeg = (parTransformB * vec4::MakeHomogeneousPositionVec4(parOnSegLocal)).xyz();
    const vec3 OnBox = (parTransformB * vec4::MakeHomogeneousPositionVec4(parOnBoxLocal)).xyz();

    ContactPoint CP;
    // Offset from medial axis toward the box along parCapsuleOffsetDir.
    const vec3 ContactOnCapsule = OnSeg - parCapsuleOffsetDir * parRadiusA;
    CP.FPosition = parInvertResult ? ContactOnCapsule : OnBox;
    CP.FPenetration = parPenetration;
    C->FManifold.FContactPoints.push_back(CP);
}

float Component(const vec3& parV, u8 parAxis)
{
    switch (parAxis)
    {
    case 0:
        return parV.x;
    case 1:
        return parV.y;
    default:
        return parV.z;
    }
}

void SetComponent(vec3& parV, u8 parAxis, float parValue)
{
    switch (parAxis)
    {
    case 0:
        parV.x = parValue;
        break;
    case 1:
        parV.y = parValue;
        break;
    default:
        parV.z = parValue;
        break;
    }
}

// Clip the capsule segment to the winning face and emit up to two contact points.
void EmitFaceManifold(Contact* C, const mat4& parTransformB, const vec3& parASLocal, const vec3& parAELocal, const vec3& parMinB, const vec3& parMaxB,
      const GeometryHelpers::SegmentAABBClosestResult& parClosest, const vec3& parCapsuleOffsetDir, const float parRadiusA, const bool parInvertResult)
{
    const u8 axis = parClosest.FaceAxis;
    const u8 uAxis = static_cast<u8>((axis + 1) % 3);
    const u8 vAxis = static_cast<u8>((axis + 2) % 3);

    const float plane = (parClosest.FaceSign > 0.f) ? Component(parMaxB, axis) : Component(parMinB, axis);
    const float uMin = Component(parMinB, uAxis);
    const float uMax = Component(parMaxB, uAxis);
    const float vMin = Component(parMinB, vAxis);
    const float vMax = Component(parMaxB, vAxis);

    const vec3 Delta = parAELocal - parASLocal;
    const float startAxis = Component(parASLocal, axis);
    const float deltaAxis = Component(Delta, axis);

    // Keep the portion of the segment whose signed plane distance is within the capsule radius
    // (outside) or on the interior side of the face.
    float tMin = 0.f;
    float tMax = 1.f;
    constexpr float kEps = 1e-6f;

    if (fabsf(deltaAxis) > kEps)
    {
        // Signed distance from plane toward the exterior: FaceSign * (coord - plane)
        // Outside contact when exteriorDistance <= radius; interior when exteriorDistance <= 0.
        const float enterLimit = parClosest.Interior ? 0.f : parRadiusA;

        // FaceSign * (startAxis + t * deltaAxis - plane) <= enterLimit
        // FaceSign * deltaAxis * t <= enterLimit - FaceSign * (startAxis - plane)
        const float faceSign = parClosest.FaceSign;
        const float rhs = enterLimit - faceSign * (startAxis - plane);
        const float coeff = faceSign * deltaAxis;

        if (fabsf(coeff) > kEps)
        {
            const float tBound = rhs / coeff;
            if (coeff > 0.f)
                tMax = Min(tMax, tBound);
            else
                tMin = Max(tMin, tBound);
        }
        else if (rhs < 0.f)
        {
            // Entire segment is outside the contact slab.
            EmitContact(C, parTransformB, parClosest.OnSegment, parClosest.OnAABB, parCapsuleOffsetDir, parRadiusA,
                  parClosest.Interior ? (parRadiusA + parClosest.ExitDepth) : (parRadiusA - sqrtf(parClosest.DistSq)), parInvertResult);
            return;
        }
    }
    else
    {
        const float exteriorDistance = parClosest.FaceSign * (startAxis - plane);
        const float enterLimit = parClosest.Interior ? 0.f : parRadiusA;
        if (exteriorDistance > enterLimit + kEps)
        {
            EmitContact(C, parTransformB, parClosest.OnSegment, parClosest.OnAABB, parCapsuleOffsetDir, parRadiusA, parRadiusA - sqrtf(parClosest.DistSq),
                  parInvertResult);
            return;
        }
    }

    if (tMin > tMax)
    {
        EmitContact(C, parTransformB, parClosest.OnSegment, parClosest.OnAABB, parCapsuleOffsetDir, parRadiusA,
              parClosest.Interior ? (parRadiusA + parClosest.ExitDepth) : (parRadiusA - sqrtf(parClosest.DistSq)), parInvertResult);
        return;
    }

    // Also clip against the face rectangle in UV so contacts stay on the winning face.
    auto ClipAgainstSlab = [&](u8 parClipAxis, float parMin, float parMax) {
        const float s = Component(parASLocal, parClipAxis);
        const float d = Component(Delta, parClipAxis);
        if (fabsf(d) <= kEps)
        {
            if (s < parMin - kEps || s > parMax + kEps)
            {
                tMin = 1.f;
                tMax = 0.f;
            }
            return;
        }

        float t1 = (parMin - s) / d;
        float t2 = (parMax - s) / d;
        if (t1 > t2)
        {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
        }
        tMin = Max(tMin, t1);
        tMax = Min(tMax, t2);
    };

    ClipAgainstSlab(uAxis, uMin, uMax);
    ClipAgainstSlab(vAxis, vMin, vMax);

    if (tMin > tMax)
    {
        EmitContact(C, parTransformB, parClosest.OnSegment, parClosest.OnAABB, parCapsuleOffsetDir, parRadiusA,
              parClosest.Interior ? (parRadiusA + parClosest.ExitDepth) : (parRadiusA - sqrtf(parClosest.DistSq)), parInvertResult);
        return;
    }

    auto MakeContactAtT = [&](float t) {
        const vec3 onSegLocal = parASLocal + Delta * t;
        vec3 onBoxLocal = onSegLocal;
        SetComponent(onBoxLocal, axis, plane);
        SetComponent(onBoxLocal, uAxis, Clamp(Component(onSegLocal, uAxis), uMin, uMax));
        SetComponent(onBoxLocal, vAxis, Clamp(Component(onSegLocal, vAxis), vMin, vMax));

        float penetration = 0.f;
        if (parClosest.Interior)
        {
            const float exitDepth = fabsf(Component(onSegLocal, axis) - plane);
            penetration = parRadiusA + exitDepth;
        }
        else
        {
            const float dist = Length(onSegLocal - onBoxLocal);
            penetration = parRadiusA - dist;
        }

        if (penetration >= 0.f)
            EmitContact(C, parTransformB, onSegLocal, onBoxLocal, parCapsuleOffsetDir, parRadiusA, penetration, parInvertResult);
    };

    constexpr float kMergeEps = 1e-4f;
    if (fabsf(tMax - tMin) <= kMergeEps)
    {
        MakeContactAtT(0.5f * (tMin + tMax));
    }
    else
    {
        MakeContactAtT(tMin);
        MakeContactAtT(tMax);
    }
}
} // namespace

bool OBBCapsuleIntersection(Contact* C,
      const mat4& parTransformA,
      const vec3& parCenterA,
      const float parRadiusA,
      const float parHalfLengthA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB,
      const bool parInvertResult)
{
    // Capsule axis is the transform Y column.
    const vec3 AxisA = parTransformA.Column(1).xyz();

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

    if (!Closest.Interior && Closest.DistSq > parRadiusA * parRadiusA)
        return false;

    // Separating normal in box-local space: from the box toward the side the capsule should be
    // pushed (outward). Exterior matches OnSeg - OnBox; interior uses the MTD face sign because
    // OnSeg - OnBox points inward when the medial axis is buried.
    vec3 SeparatingNormalLocal;
    if (Closest.Interior)
    {
        SeparatingNormalLocal = vec3(0.f);
        SetComponent(SeparatingNormalLocal, Closest.FaceAxis, Closest.FaceSign);
    }
    else if (Closest.DistSq > 1e-6f)
    {
        SeparatingNormalLocal = (Closest.OnSegment - Closest.OnAABB) / sqrtf(Closest.DistSq);
    }
    else
    {
        SeparatingNormalLocal = vec3(0.f);
        SetComponent(SeparatingNormalLocal, Closest.FaceAxis, Closest.FaceSign);
        if (LengthSq(SeparatingNormalLocal) <= 1e-6f)
            SeparatingNormalLocal = vec3(0.f, 1.f, 0.f);
    }

    const vec3 NormalBoxToCap = (parTransformB * vec4::MakeHomogeneousDirectionVec4(SeparatingNormalLocal)).xyz();
    C->FContactNormal = parInvertResult ? Invert(NormalBoxToCap) : NormalBoxToCap;

    // Direction used as OnSeg - offsetDir * radius to reach the capsule surface facing the box.
    // Interior needs the opposite of the separating normal so the offset moves toward the exit face.
    const vec3 CapsuleOffsetDir = Closest.Interior ? Invert(NormalBoxToCap) : NormalBoxToCap;

    if (Closest.Feature == GeometryHelpers::ESegmentAABBFeature::Face || Closest.Feature == GeometryHelpers::ESegmentAABBFeature::Interior)
    {
        EmitFaceManifold(C, parTransformB, ASLocal, AELocal, MinB, MaxB, Closest, CapsuleOffsetDir, parRadiusA, parInvertResult);
    }
    else
    {
        const float penetration = Closest.Interior ? (parRadiusA + Closest.ExitDepth) : (parRadiusA - sqrtf(Closest.DistSq));
        EmitContact(C, parTransformB, Closest.OnSegment, Closest.OnAABB, CapsuleOffsetDir, parRadiusA, penetration, parInvertResult);
    }

    return !C->FManifold.FContactPoints.empty();
}
} // namespace Physics
} // namespace ECSEngine
