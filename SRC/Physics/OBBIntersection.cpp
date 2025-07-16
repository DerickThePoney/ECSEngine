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
      const AABB3f& parBoundingBoxB)
{
    vec4 Axis = parTransformA.Column(parAxisIndex);
    if (Dot(Axis, parBoxesCenterSeparation) > 0.f)
    {
        Axis *= -1.f;
    }

    // contact vertex in local b box coordinates
    vec4 sign = parBoxesCenterSeparation / Abs(parBoxesCenterSeparation);
    vec4 contactVertex = vec4::MakeHomogeneousPositionVec4(parBoundingBoxB.Max());
    if (Dot(parTransformB.Column(0), Axis) < 0.f)
        contactVertex.x = -contactVertex.x;
    if (Dot(parTransformB.Column(1), Axis) < 0.f)
        contactVertex.y = -contactVertex.y;
    if (Dot(parTransformB.Column(2), Axis) < 0.f)
        contactVertex.z = -contactVertex.z;

    /*sign.w = -1.f;
    contactVertex = contactVertex * Invert(sign);*/
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
    if (Dot(Axis, parBoxesCenterSeparation))
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
        ComputeFaceAxisIntersectionData(C, parOverlap, parAxisIndex, parBoxesCenterSeparation, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    }
    else if (parAxisIndex < 6)
    {
        ComputeFaceAxisIntersectionData(C, parOverlap, parAxisIndex - 3, Invert(parBoxesCenterSeparation), parTransformB, parBoundingBoxB, parTransformA, parBoundingBoxA);
    }
    else
    {
        ComputeEdgeEdgeIntersectionData(C, parOverlap, parAxisIndex - 6, parBoxesCenterSeparation, parTransformA, parBoundingBoxA, parTransformB, parBoundingBoxB);
    }

    C->FFlags.SetBit(EContactFlag::CONTACT_INFO, true);
}

bool OBBIntersection(Contact* C, const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB)
{
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

    return true;
}

} // namespace Physics
} // namespace ECSEngine