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

void EmitContactPoint(Contact* C,
      const mat4& parTransformB,
      const vec3& parOnSegLocal,
      const vec3& parOnBoxLocal,
      const float parRadiusA,
      const float parPenetration,
      const bool parInvertResult)
{
    const vec3 OnSeg = (parTransformB * vec4::MakeHomogeneousPositionVec4(parOnSegLocal)).xyz();
    const vec3 OnBox = (parTransformB * vec4::MakeHomogeneousPositionVec4(parOnBoxLocal)).xyz();

    ContactPoint CP;
    // Capsule as body A: same convention as CapsuleSphereIntersection (offset along final normal).
    // Box as body A: contact on the box surface (same as OBBSphereIntersection).
    CP.FPosition = parInvertResult ? (OnSeg + C->FContactNormal * parRadiusA) : OnBox;
    CP.FPenetration = parPenetration;
    C->FManifold.FContactPoints.push_back(CP);
}

// When the capsule lies along a face, clip the segment to that face and emit the two ends of the
// overlap. End-on / edge / vertex hits keep a single closest contact instead.
bool TryEmitFaceManifold(Contact* C,
      const mat4& parTransformB,
      const vec3& parASLocal,
      const vec3& parAELocal,
      const vec3& parMinB,
      const vec3& parMaxB,
      const GeometryHelpers::SegmentAABBClosestResult& parClosest,
      const float parRadiusA,
      const bool parInvertResult)
{
    if (parClosest.Feature != GeometryHelpers::ESegmentAABBFeature::Face && parClosest.Feature != GeometryHelpers::ESegmentAABBFeature::Interior)
        return false;

    const u8 axis = parClosest.FaceAxis;
    const u8 uAxis = static_cast<u8>((axis + 1) % 3);
    const u8 vAxis = static_cast<u8>((axis + 2) % 3);

    const float plane = (parClosest.FaceSign > 0.f) ? Component(parMaxB, axis) : Component(parMinB, axis);
    const float uMin = Component(parMinB, uAxis);
    const float uMax = Component(parMaxB, uAxis);
    const float vMin = Component(parMinB, vAxis);
    const float vMax = Component(parMaxB, vAxis);

    const vec3 Delta = parAELocal - parASLocal;
    const float segmentLenSq = LengthSq(Delta);
    if (segmentLenSq <= 1e-8f)
        return false;

    // Two contacts only when the capsule is lying on the face: axis nearly perpendicular
    // to the face normal. Any meaningful end-on component → keep a single closest contact
    // so the manifold does not shift while the capsule is still tipping.
    const float axisAlongNormal = fabsf(Component(Delta, axis)) / sqrtf(segmentLenSq);
    constexpr float kMaxAlignWithNormal = 0.05f; // nearly perpendicular to the face normal
    if (axisAlongNormal > kMaxAlignWithNormal)
        return false;

    // Clip only against the face rectangle (UV). Do NOT shrink the span with a plane-distance
    // slab here: a slight tip would clip one end and leave both contacts on the low half,
    // which torques the capsule. Per-point radius checks below reject ends that are too far.
    float tMin = 0.f;
    float tMax = 1.f;
    constexpr float kEps = 1e-6f;

    auto ClipAgainstSlab = [&](u8 parClipAxis, float parMin, float parMax)
    {
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
        return false;

    // Need real support on either side of the capsule center (not a barely-straddling pair).
    constexpr float kCenterT = 0.5f;
    constexpr float kMinSideT = 0.2f; // >= 20% of segment length on each side of center
    if (tMin > kCenterT - kMinSideT || tMax < kCenterT + kMinSideT)
        return false;

    auto BuildPointAtT = [&](float t, vec3& outOnSeg, vec3& outOnBox, float& outPenetration) -> bool
    {
        outOnSeg = parASLocal + Delta * t;
        outOnBox = outOnSeg;
        SetComponent(outOnBox, axis, plane);
        SetComponent(outOnBox, uAxis, Clamp(Component(outOnSeg, uAxis), uMin, uMax));
        SetComponent(outOnBox, vAxis, Clamp(Component(outOnSeg, vAxis), vMin, vMax));

        // Inside the solid: depth from the exit face.
        const float exteriorDistance = parClosest.FaceSign * (Component(outOnSeg, axis) - plane);
        if (exteriorDistance < 0.f)
        {
            outPenetration = -(parRadiusA - exteriorDistance); // radius + depth
            // Keep the box point on the face plane for a stable normal/position.
            SetComponent(outOnBox, axis, plane);
            return true;
        }

        const float dist = Length(outOnSeg - outOnBox);
        if (dist > parRadiusA + 1e-4f)
            return false;
        outPenetration = dist - parRadiusA;
        return true;
    };

    vec3 onSeg0, onBox0, onSeg1, onBox1;
    float pen0 = 0.f, pen1 = 0.f;
    // Extremes of the face overlap → one contact toward each end when both are in range.
    if (!BuildPointAtT(tMin, onSeg0, onBox0, pen0) || !BuildPointAtT(tMax, onSeg1, onBox1, pen1))
        return false;

    EmitContactPoint(C, parTransformB, onSeg0, onBox0, parRadiusA, pen0, parInvertResult);
    EmitContactPoint(C, parTransformB, onSeg1, onBox1, parRadiusA, pen1, parInvertResult);
    return true;
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
    const vec3 AxisA = Normalize(parTransformA.Column(1).xyz());

    const vec3 CenterAW = (parTransformA * vec4::MakeHomogeneousPositionVec4(parCenterA)).xyz();
    const vec3 AS = CenterAW - AxisA * parHalfLengthA;
    const vec3 AE = CenterAW + AxisA * parHalfLengthA;

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

    vec3 NormalBoxToCap = (parTransformB * vec4::MakeHomogeneousDirectionVec4(SeparatingNormalLocal)).xyz();
    const float NormalLenSq = LengthSq(NormalBoxToCap);
    if (NormalLenSq > 1e-12f)
        NormalBoxToCap = NormalBoxToCap / sqrtf(NormalLenSq);
    else
        NormalBoxToCap = vec3(0.f, 1.f, 0.f);

    C->FContactNormal = parInvertResult ? Invert(NormalBoxToCap) : NormalBoxToCap;

    // Lying on a face → up to 2 contacts at the ends of the face overlap.
    // Edge / vertex / end-on → fall through to a single closest contact.
    if (TryEmitFaceManifold(C, parTransformB, ASLocal, AELocal, MinB, MaxB, Closest, parRadiusA, parInvertResult))
        return !C->FManifold.FContactPoints.empty();

    const float penetration = Closest.Interior ? -(parRadiusA + Closest.ExitDepth) : (sqrtf(Closest.DistSq) - parRadiusA);
    EmitContactPoint(C, parTransformB, Closest.OnSegment, Closest.OnAABB, parRadiusA, penetration, parInvertResult);
    return true;
}
} // namespace Physics
} // namespace ECSEngine
