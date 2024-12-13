#include "stdafx.h"

#include "OBBIntersection.h"

#include "Common/FixedSizedArray.h"
#include "Common/MemoryView.h"
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
      MemoryView<vec4>& parCornersB)
{
    const float ProjectionOnBoxA = GeometryHelpers::ProjectBoxToAxis(parAxis, CenterA, parCornersA);
    const float ProjectionOnBoxB = GeometryHelpers::ProjectBoxToAxis(parAxis, CenterB, parCornersB);

    const float distance = fabs(Dot(parBoxesCenterSeparation, parAxis));

    return ProjectionOnBoxA + ProjectionOnBoxB - distance;
}

bool OBBIntersection(const mat4& parTransformA, const AABB3f& parBoundingBoxA, const mat4& parTransformB, const AABB3f& parBoundingBoxB)
{
    vec4 CenterA = parTransformA * vec4::MakeHomogeneousPositionVec4(parBoundingBoxA.Center());
    vec4 CenterB = parTransformB * vec4::MakeHomogeneousPositionVec4(parBoundingBoxB.Center());
    vec4 BoxesCenterSeparation = CenterA - CenterB;

    // Extract Corners
    FixedSizedArrayInSitu<vec4, 8> CornersA;
    MemoryView<vec4> CornersAView = MemoryView<vec4>(CornersA.data(), CornersA.size());
    GeometryHelpers::ExtractOBBCorners(parBoundingBoxA, parTransformA, CornersAView);
    FixedSizedArrayInSitu<vec4, 8> CornersB;
    MemoryView<vec4> CornersBView = MemoryView<vec4>(CornersB.data(), CornersB.size());
    GeometryHelpers::ExtractOBBCorners(parBoundingBoxB, parTransformB, CornersBView);

    // Box A Axes
    float res = PenetrationOnAxis(parTransformA.Column(0), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    res = PenetrationOnAxis(parTransformA.Column(1), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    res = PenetrationOnAxis(parTransformA.Column(2), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    // Box B Axes
    res = PenetrationOnAxis(parTransformB.Column(0), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    res = PenetrationOnAxis(parTransformB.Column(1), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    res = PenetrationOnAxis(parTransformB.Column(2), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
    if (res < 0.f)
        return false;

    // Axes pairs
    forrange(i, 0, 3)
    {
        forrange(j, 0, 3)
        {
            const vec4 axis = vec4::MakeHomogeneousDirectionVec4(Cross(parTransformA.Column(i).xyz(), parTransformB.Column(j).xyz()));

            if (LengthSq(axis) < 0.00001f) // parallel axes
                continue;

            res = PenetrationOnAxis(Normalize(axis), BoxesCenterSeparation, CenterA, CenterB, CornersAView, CornersBView);
            if (res < 0.f)
                return false;
        }
    }

    return true;
}

} // namespace Physics
} // namespace ECSEngine