#include "stdafx.h"

#include "OBBIntersection.h"

#include "Common/FixedSizedArray.h"
#include "Common/MemoryView.h"
#include "Contact.h"
#include "GeometryHelpers.h"

namespace ECSEngine
{
namespace Physics
{
float PenetrationOnAxis(const vec4& parAxis,
      const vec4& parBoxesCenterSeparation,
      const vec4& CenterA,
      const vec4& CenterB,
      MemoryView<vec4>& parCornersA,
      MemoryView<vec4>& parCornersB,
      const mat4& parTransformA,
      const AABB3f& parBBoxA,
      const mat4& parTransformB,
      const AABB3f& parBBoxB)
{
    const float ProjectionOnBoxA = GeometryHelpers::ProjectBoxToAxis(parAxis, parTransformA, parBBoxA);
    const float ProjectionOnBoxB = GeometryHelpers::ProjectBoxToAxis(parAxis, parTransformB, parBBoxB);

    const float distance = fabs(Dot(parBoxesCenterSeparation, parAxis));

    // std::cout << fmt::format("axis: x={} y={} z={} -> pen={}\n", parAxis.x, parAxis.y, parAxis.z, ProjectionOnBoxA + ProjectionOnBoxB - distance);
    return ProjectionOnBoxA + ProjectionOnBoxB - distance;
}

void ComputeFaceAxisIntersectionData(Contact* C,
      const float parOverlap,
      const i32 parAxisIndex,
      const vec4& parBoxesCenterSeparation,
      const mat4& parTransformA,
      const AABB3f& parBoundingBoxA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB,
      const bool bShouldFlip)
{
    vec4 Axis = parTransformA.Column(parAxisIndex);
    if (Dot(Axis, parBoxesCenterSeparation) > 0.f)
    {
        Axis *= -1.f;
    }

#ifdef PERFORM_SECURITY_CHECKS
    C->SeparatingAxis = Axis.xyz();
    C->SeparationVector = parBoxesCenterSeparation.xyz();
#endif

    // contact vertex in local b box coordinates
    vec4 sign = parBoxesCenterSeparation / Abs(parBoxesCenterSeparation);
    vec4 contactVertex = vec4::MakeHomogeneousPositionVec4(parBoundingBoxB.Max());
    if (Dot(parTransformB.Column(0), Axis) < 0.f)
        contactVertex.x = -contactVertex.x;
    if (Dot(parTransformB.Column(1), Axis) < 0.f)
        contactVertex.y = -contactVertex.y;
    if (Dot(parTransformB.Column(2), Axis) < 0.f)
        contactVertex.z = -contactVertex.z;

    if (bShouldFlip)
    {
        sign.w = -1.f;
        contactVertex = contactVertex * (-1.f * sign);
    }
    // contact vertex in world coordinate
    contactVertex = parTransformB * contactVertex;

    C->FContactNormal = Axis.xyz();
    C->FContactPoint = contactVertex.xyz();
    C->FPenetration = parOverlap;
}

void VectorIndexSetterHelper(vec3& vec, i32 index, float value)
{
    switch (index)
    {
    case 0:
        vec.x = value;
        return;
    case 1:
        vec.y = value;
        return;
    case 2:
        vec.z = value;
        return;
    default:
        break;
    }
}

float VectorIndexGetterHelper(const vec3& vec, i32 index)
{
    switch (index)
    {
    case 0:
        return vec.x;
    case 1:
        return vec.y;
    case 2:
        return vec.z;
    default:
        AssertNotReached();
        return -1;
    }
}

vec3 ComputeContactPoint(const vec3& pOne,
      const vec3& dOne,
      float oneSize,
      const vec3& pTwo,
      const vec3& dTwo,
      float twoSize,

      // If this is true, and the contact point is outside
      // the edge (in the case of an edge-face contact) then
      // we use one's midpoint, otherwise we use two's.
      bool useOne)
{
    vec3 toSt, cOne, cTwo;
    float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
    float denom, mua, mub;

    smOne = LengthSq(dOne);
    smTwo = LengthSq(dTwo);
    dpOneTwo = Dot(dTwo, dOne);

    toSt = pOne - pTwo;
    dpStaOne = Dot(dOne, toSt);
    dpStaTwo = Dot(dTwo, toSt);

    denom = smOne * smTwo - dpOneTwo * dpOneTwo;

    // Zero denominator indicates parrallel lines
    if (fabs(denom) < 0.0001f)
    {
        return useOne ? pOne : pTwo;
    }

    mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
    mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

    // If either of the edges has the nearest point out
    // of bounds, then the edges aren't crossed, we have
    // an edge-face contact. Our point is on the edge, which
    // we know from the useOne parameter.
    if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize)
    {
        return useOne ? pOne : pTwo;
    }
    else
    {
        cOne = pOne + dOne * mua;
        cTwo = pTwo + dTwo * mub;

        return cOne * 0.5 + cTwo * 0.5;
    }
}

void ComputeEdgeEdgeIntersectionData(Contact* C,
      const float parOverlap,
      const i32 parAxisIndex,
      const vec4& parBoxesCenterSeparation,
      const mat4& parTransformA,
      const AABB3f& parBoundingBoxA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB)
{
    const i32 BoxAAxisIndex = parAxisIndex / 3;
    const i32 BoxBAxisIndex = parAxisIndex % 3;

    const vec4 AxisA = parTransformA.Column(BoxBAxisIndex);
    const vec4 AxisB = parTransformB.Column(BoxBAxisIndex);
    vec4 Axis = vec4::MakeHomogeneousDirectionVec4(Normalize(Cross(AxisA.xyz(), AxisB.xyz())));
    if (IsNan(Axis))
        Axis = AxisA;
    if (Dot(Axis, parBoxesCenterSeparation) > 0)
    {
        Axis = -1.f * Axis;
    }

    vec3 pointOnA = parBoundingBoxA.Max();
    vec3 pointOnB = parBoundingBoxB.Max();

    forrange(i, 0, 3)
    {
        if (i == BoxAAxisIndex)
            VectorIndexSetterHelper(pointOnA, i, 0.f);
        else if (Dot(parTransformA.Column(i), Axis) > 0.f)
        {
            VectorIndexSetterHelper(pointOnA, i, -VectorIndexGetterHelper(pointOnA, i));
        }

        if (i == BoxBAxisIndex)
            VectorIndexSetterHelper(pointOnB, i, 0.f);
        else if (Dot(parTransformB.Column(i), Axis) > 0.f)
        {
            VectorIndexSetterHelper(pointOnB, i, -VectorIndexGetterHelper(pointOnB, i));
        }
    }

    vec4 pointOnAV4 = parTransformA * vec4::MakeHomogeneousPositionVec4(pointOnA);
    vec4 pointOnBV4 = parTransformB * vec4::MakeHomogeneousPositionVec4(pointOnB);

    C->FContactPoint = ComputeContactPoint(pointOnAV4.xyz(), AxisA.xyz(), VectorIndexGetterHelper(parBoundingBoxA.Max(), BoxAAxisIndex), pointOnBV4.xyz(), AxisB.xyz(),
          VectorIndexGetterHelper(parBoundingBoxB.Max(), BoxBAxisIndex), parAxisIndex > 2);
    C->FContactNormal = Axis.xyz();
    C->FPenetration = parOverlap;
}

void ComputeContactIntersectionData(Contact* C,
      const float parOverlap,
      const i32 parAxisIndex,
      const vec4& parBoxesCenterSeparation,
      const mat4& parTransformA,
      const AABB3f& parBoundingBoxA,
      const mat4& parTransformB,
      const AABB3f& parBoundingBoxB)
{
    if (parAxisIndex < 3)
    {
        ComputeFaceAxisIntersectionData(C, parOverlap, parAxisIndex, parBoxesCenterSeparation, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB, false);
    }
    else if (parAxisIndex < 6)
    {
        ComputeFaceAxisIntersectionData(C, parOverlap, parAxisIndex - 3, Invert(parBoxesCenterSeparation), parTransformB, parBoundingBoxB, parTransformA, parBoundingBoxA, true);
    }
    else
    {
        ComputeEdgeEdgeIntersectionData(C, parOverlap, parAxisIndex - 6, parBoxesCenterSeparation, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    }

    C->FFlags.SetBit(EContactFlag::CONTACT_INFO, true);
}
//--------------------------------------------------------------------------------------------------
inline void EdgesContact(vec4* CA, vec4* CB, const vec4& PA, const vec4& QA, const vec4& PB, const vec4& QB)
{
    vec4 DA = QA - PA;
    vec4 DB = QB - PB;
    vec4 r = PA - PB;
    float a = Dot(DA, DA);
    float e = Dot(DB, DB);
    float f = Dot(DB, r);
    float c = Dot(DA, r);

    float b = Dot(DA, DB);
    float denom = a * e - b * b;

    float TA = (b * f - c * e) / denom;
    float TB = (b * TA + f) / e;

    *CA = PA + DA * TA;
    *CB = PB + DB * TB;
}
//--------------------------------------------------------------------------------------------------
void SupportEdge(const mat4& tx, const vec4& e, vec4 n, vec4* aOut, vec4* bOut)
{
    n = GetRotationAsMat4(tx) * n;
    vec4 absN = Abs(n);
    vec4 a, b;

    // x > y
    if (absN.x > absN.y)
    {
        // x > y > z
        if (absN.y > absN.z)
        {
            a = vec4(e.x, e.y, e.z);
            b = vec4(e.x, e.y, -e.z);
        }

        // x > z > y || z > x > y
        else
        {
            a = vec4(e.x, e.y, e.z);
            b = vec4(e.x, -e.y, e.z);
        }
    }

    // y > x
    else
    {
        // y > x > z
        if (absN.x > absN.z)
        {
            a = vec4(e.x, e.y, e.z);
            b = vec4(e.x, e.y, -e.z);
        }

        // z > y > x || y > z > x
        else
        {
            a = vec4(e.x, e.y, e.z);
            b = vec4(-e.x, e.y, e.z);
        }
    }

    float signx = Sign(n.x);
    float signy = Sign(n.y);
    float signz = Sign(n.z);

    a.x *= signx;
    a.y *= signy;
    a.z *= signz;
    b.x *= signx;
    b.y *= signy;
    b.z *= signz;

    *aOut = tx * a;
    *bOut = tx * b;
}

bool TrackFaceAxis(i32* axis, i32 n, float Penetration, float* PenetrationMax, const vec4& normal, vec4* axisNormal)
{
    if (Penetration > 0.f)
        return true;

    if (Penetration > *PenetrationMax)
    {
        *PenetrationMax = Penetration;
        *axis = n;
        *axisNormal = normal;
    }

    return false;
}

bool OBBIntersection(Contact* C, const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB)
{
    mat4 RotationA = GetRotationAsMat4(parTransformA);
    mat4 RotationB = GetRotationAsMat4(parTransformB);

    vec4 ExtentsA = vec4::MakeHomogeneousDirectionVec4(parBoundingBoxA.Extent());
    vec4 ExtentsB = vec4::MakeHomogeneousDirectionVec4(parBoundingBoxB.Extent());

    // Extract B in A's Frame
    mat4 BinAFrame = Transpose(RotationA) * RotationB;

    bool bParallel = false;
    constexpr float kCosTol = 1e-6f;
    mat4 AbsBinAFrame;
    forrange(i, 0, 3)
    {
        vec4 ithColumn = BinAFrame.Column(i);
        ithColumn.x = abs(ithColumn.x);
        if (ithColumn.x + kCosTol >= 1.f)
            bParallel = true;

        ithColumn.y = abs(ithColumn.y);
        if (ithColumn.y + kCosTol >= 1.f)
            bParallel = true;

        ithColumn.z = abs(ithColumn.z);
        if (ithColumn.z + kCosTol >= 1.f)
            bParallel = true;

        AbsBinAFrame.SetColumn(i, ithColumn);
    }

    // Get the separation vector in A's frame
    vec4 SeparationVector = RotationA *
          (parTransformB * vec4::MakeHomogeneousPositionVec4(parBoundingBoxB.Center()) - parTransformA * vec4::MakeHomogeneousPositionVec4(parBoundingBoxA.Center()));

    // Query states
    float Penetration = 0.f;
    float PenetrationAMax = -std::numeric_limits<float>().max();
    float PenetrationBMax = -std::numeric_limits<float>().max();
    float PenetrationEMax = -std::numeric_limits<float>().max();
    i32 AAxis = ~0;
    i32 BAxis = ~0;
    i32 EAxis = ~0;
    vec4 NormalA;
    vec4 NormalB;
    vec4 NormalE;

    // checking face axis
    // A's X axis
    Penetration = abs(SeparationVector.x) - (ExtentsA.x + Dot(AbsBinAFrame.Column(0), ExtentsB));
    if (TrackFaceAxis(&AAxis, 0, Penetration, &PenetrationAMax, RotationA.Column(0), &NormalA))
        return false;

    // A's Y axis
    Penetration = abs(SeparationVector.y) - (ExtentsA.y + Dot(AbsBinAFrame.Column(1), ExtentsB));
    if (TrackFaceAxis(&AAxis, 1, Penetration, &PenetrationAMax, RotationA.Column(1), &NormalA))
        return false;

    // A's Z axis
    Penetration = abs(SeparationVector.z) - (ExtentsA.z + Dot(AbsBinAFrame.Column(2), ExtentsB));
    if (TrackFaceAxis(&AAxis, 2, Penetration, &PenetrationAMax, RotationA.Column(2), &NormalA))
        return false;

    // B's X axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(0))) - (ExtentsB.x + Dot(AbsBinAFrame.Column(0), ExtentsA));
    if (TrackFaceAxis(&BAxis, 3, Penetration, &PenetrationBMax, RotationB.Column(0), &NormalB))
        return false;

    // B's Y axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(1))) - (ExtentsB.y + Dot(AbsBinAFrame.Column(1), ExtentsA));
    if (TrackFaceAxis(&BAxis, 4, Penetration, &PenetrationBMax, RotationB.Column(1), &NormalB))
        return false;

    // B's Z axis
    Penetration = abs(Dot(SeparationVector, BinAFrame.Column(2))) - (ExtentsB.z + Dot(AbsBinAFrame.Column(2), ExtentsA));
    if (TrackFaceAxis(&BAxis, 5, Penetration, &PenetrationBMax, RotationB.Column(2), &NormalB))
        return false;

    if (!bParallel)
    {
        // Edge axis checks
        float rA;
        float rB;

        // Cross( a.x, b.x )
        rA = ExtentsA.y * AbsBinAFrame.Column(2).x + ExtentsA.z * AbsBinAFrame.Column(1).x;
        rB = ExtentsB.y * AbsBinAFrame.Column(0).z + ExtentsB.z * AbsBinAFrame.Column(0).y;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(1).x - SeparationVector.y * BinAFrame.Column(2).x) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 6, Penetration, &PenetrationEMax, vec4(0.f, -BinAFrame.Column(2).x, BinAFrame.Column(1).x, 0.f), &NormalE))
            return false;

        // Cross( a.x, b.y )
        rA = ExtentsA.y * AbsBinAFrame.Column(2).y + ExtentsA.z * AbsBinAFrame.Column(1).y;
        rB = ExtentsB.x * AbsBinAFrame.Column(0).z + ExtentsB.z * AbsBinAFrame.Column(0).x;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(1).y - SeparationVector.y * BinAFrame.Column(2).y) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 7, Penetration, &PenetrationEMax, vec4(0.f, -BinAFrame.Column(2).y, BinAFrame.Column(1).y), &NormalE))
            return false;

        // Cross( a.x, b.z )
        rA = ExtentsA.y * AbsBinAFrame.Column(2).z + ExtentsA.z * AbsBinAFrame.Column(1).z;
        rB = ExtentsB.x * AbsBinAFrame.Column(0).y + ExtentsB.y * AbsBinAFrame.Column(0).x;
        Penetration = abs(SeparationVector.z * BinAFrame.Column(1).z - SeparationVector.y * BinAFrame.Column(2).z) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 8, Penetration, &PenetrationEMax, vec4(0.f, -BinAFrame.Column(2).z, BinAFrame.Column(1).z), &NormalE))
            return false;

        // Cross( a.y, b.x )
        rA = ExtentsA.x * AbsBinAFrame.Column(2).x + ExtentsA.z * AbsBinAFrame.Column(0).x;
        rB = ExtentsB.y * AbsBinAFrame.Column(1).z + ExtentsB.z * AbsBinAFrame.Column(1).y;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(2).x - SeparationVector.z * BinAFrame.Column(0).x) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 9, Penetration, &PenetrationEMax, vec4(BinAFrame.Column(2).x, 0.f, -BinAFrame.Column(0).x), &NormalE))
            return false;

        // Cross( a.y, b.y )
        rA = ExtentsA.x * AbsBinAFrame.Column(2).y + ExtentsA.z * AbsBinAFrame.Column(0).y;
        rB = ExtentsB.x * AbsBinAFrame.Column(1).z + ExtentsB.z * AbsBinAFrame.Column(1).x;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(2).y - SeparationVector.z * BinAFrame.Column(0).y) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 10, Penetration, &PenetrationEMax, vec4(BinAFrame.Column(2).y, 0.f, -BinAFrame.Column(0).y), &NormalE))
            return false;

        // Cross( a.y, b.z )
        rA = ExtentsA.x * AbsBinAFrame.Column(2).z + ExtentsA.z * AbsBinAFrame.Column(0).z;
        rB = ExtentsB.x * AbsBinAFrame.Column(1).y + ExtentsB.y * AbsBinAFrame.Column(1).x;
        Penetration = abs(SeparationVector.x * BinAFrame.Column(2).z - SeparationVector.z * BinAFrame.Column(0).z) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 11, Penetration, &PenetrationEMax, vec4(BinAFrame.Column(2).z, 0.f, -BinAFrame.Column(0).z), &NormalE))
            return false;

        // Cross( a.z, b.x )
        rA = ExtentsA.x * AbsBinAFrame.Column(1).x + ExtentsA.y * AbsBinAFrame.Column(0).x;
        rB = ExtentsB.y * AbsBinAFrame.Column(2).z + ExtentsB.z * AbsBinAFrame.Column(2).y;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(0).x - SeparationVector.x * BinAFrame.Column(1).x) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 12, Penetration, &PenetrationEMax, vec4(-BinAFrame.Column(1).x, BinAFrame.Column(0).x, 0.f), &NormalE))
            return false;

        // Cross( a.z, b.y )
        rA = ExtentsA.x * AbsBinAFrame.Column(1).y + ExtentsA.y * AbsBinAFrame.Column(0).y;
        rB = ExtentsB.x * AbsBinAFrame.Column(2).z + ExtentsB.z * AbsBinAFrame.Column(2).x;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(0).y - SeparationVector.x * BinAFrame.Column(1).y) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 13, Penetration, &PenetrationEMax, vec4(-BinAFrame.Column(1).y, BinAFrame.Column(0).y, 0.f), &NormalE))
            return false;

        // Cross( a.z, b.z )
        rA = ExtentsA.x * AbsBinAFrame.Column(1).z + ExtentsA.y * AbsBinAFrame.Column(0).z;
        rB = ExtentsB.x * AbsBinAFrame.Column(2).y + ExtentsB.y * AbsBinAFrame.Column(2).x;
        Penetration = abs(SeparationVector.y * BinAFrame.Column(0).z - SeparationVector.x * BinAFrame.Column(1).z) - (rA + rB);
        if (TrackFaceAxis(&EAxis, 14, Penetration, &PenetrationEMax, vec4(-BinAFrame.Column(1).z, BinAFrame.Column(0).z, 0.f), &NormalE))
            return false;
    }

    // Artificial axis bias
    static constexpr float kRelTol = 0.95f;
    static constexpr float kAbsTol = 0.01f;
    i32 Axis;
    float MaxPenetration;
    vec4 Normal;

    float FacePenetrationMax = Max(PenetrationAMax, PenetrationBMax);
    if (kRelTol * PenetrationEMax > FacePenetrationMax + kAbsTol)
    {
        Axis = EAxis;
        MaxPenetration = PenetrationEMax;
        Normal = NormalE;
    }
    else
    {
        if (kRelTol * PenetrationBMax > PenetrationAMax + kAbsTol)
        {
            Axis = BAxis;
            MaxPenetration = PenetrationBMax;
            Normal = NormalB;
        }
        else
        {
            Axis = AAxis;
            MaxPenetration = PenetrationAMax;
            Normal = NormalA;
        }
    }

    if (Dot(Normal, parTransformB.Column(3) - parTransformA.Column(3)) < 0.f)
    {
        Normal = Invert(Normal);
    }

    if (Axis == ~0)
    {
        return false;
    }

    if (Axis < 6)
    {
        // TODO
    }
    else
    {
        Normal = RotationA * Normal;

        if (Dot(Normal, parTransformB.Column(3) - parTransformA.Column(3)) < 0.f)
            Normal = Invert(Normal);

        vec4 PA, QA;
        vec4 PB, QB;
        SupportEdge(parTransformA, ExtentsA, Normal, &PA, &QA);
        SupportEdge(parTransformB, ExtentsB, Invert(Normal), &PB, &QB);

        vec4 CA, CB;
        EdgesContact(&CA, &CB, PA, QA, PB, QB);

        C->FContactNormal = Normal.xyz();
        /*m->contactCount = 1;*/

        /*q3Contact* c = m->contacts;
        q3FeaturePair pair;*/
        /*pair.key = axis;*/
        /*C->fp = pair;*/
        C->FPenetration = Penetration;
        C->FContactPoint = ((CA + CB) * 0.5f).xyz();
    }

    C->FFlags.SetBit(EContactFlag::CONTACT_INFO, true);

#if 0
    vec4 CenterA = parTransformA * vec4::MakeHomogeneousPositionVec4(parBoundingBoxA.Center());
    vec4 CenterB = parTransformB * vec4::MakeHomogeneousPositionVec4(parBoundingBoxB.Center());
    vec4 BoxesCenterSeparationAB = CenterB - CenterA;
    vec4 BoxesCenterSeparationBA = CenterA - CenterB;

    // Extract Corners
    FixedSizedArrayInSitu<vec4, 8> CornersA;
    MemoryView<vec4> CornersAView = MemoryView<vec4>(CornersA.data(), CornersA.size());
    GeometryHelpers::ExtractOBBCorners(parBoundingBoxA, parTransformA, CornersAView);
    FixedSizedArrayInSitu<vec4, 8> CornersB;
    MemoryView<vec4> CornersBView = MemoryView<vec4>(CornersB.data(), CornersB.size());
    GeometryHelpers::ExtractOBBCorners(parBoundingBoxB, parTransformB, CornersBView);

    float MinPenetration = std::numeric_limits<float>::max();
    i32 MinPenetrationIdx = -1;
    i32 CurrentIndex = -1;
    // Box A Axes
    CurrentIndex++;
    float res = PenetrationOnAxis(
          parTransformA.Column(0), BoxesCenterSeparationAB, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    CurrentIndex++;
    res = PenetrationOnAxis(
          parTransformA.Column(1), BoxesCenterSeparationAB, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    CurrentIndex++;
    res = PenetrationOnAxis(
          parTransformA.Column(2), BoxesCenterSeparationAB, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    // Box B Axes
    CurrentIndex++;
    res = PenetrationOnAxis(
          parTransformB.Column(0), BoxesCenterSeparationBA, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    CurrentIndex++;
    res = PenetrationOnAxis(
          parTransformB.Column(1), BoxesCenterSeparationBA, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    CurrentIndex++;
    res = PenetrationOnAxis(
          parTransformB.Column(2), BoxesCenterSeparationBA, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    if (res < 0.f)
        return false;
    if (res < MinPenetration)
    {
        MinPenetration = res;
        MinPenetrationIdx = CurrentIndex;
    }

    // Axes pairs
    forrange(i, 0, 3)
    {
        forrange(j, 0, 3)
        {
            CurrentIndex++;
            const vec4 axis = vec4::MakeHomogeneousDirectionVec4(Cross(parTransformA.Column(i).xyz(), parTransformB.Column(j).xyz()));

            if (LengthSq(axis) < 0.00001f) // parallel axes
                continue;

            res = PenetrationOnAxis(
                  Normalize(axis), BoxesCenterSeparationAB, CenterA, CenterB, CornersAView, CornersBView, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
            if (res < 0.f)
                return false;
            if (res < MinPenetration)
            {
                MinPenetration = res;
                MinPenetrationIdx = CurrentIndex;
            }
        }
    }

    ComputeContactIntersectionData(C, MinPenetration, MinPenetrationIdx, BoxesCenterSeparationAB, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
#endif
    return true;
}

} // namespace Physics
} // namespace ECSEngine