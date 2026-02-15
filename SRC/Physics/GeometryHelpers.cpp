#include "stdafx.h"

#include "GeometryHelpers.h"

#include "Common/FixedSizedArray.h"
#include "Math/VectorUtils.h"

namespace ECSEngine
{
namespace Physics
{
namespace GeometryHelpers
{

void ExtractAABBCorners(const AABB3f& OBB, MemoryView<vec4>& parView)
{
    AssertRelease(parView.size() >= 8);
    const vec3& obbMin = OBB.Min();
    const vec3& obbMax = OBB.Max();
    parView[0] = vec4::MakeHomogeneousPositionVec4(obbMin);
    parView[1] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMin.y, obbMax.z));
    parView[2] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMin.y, obbMax.z));
    parView[3] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMin.y, obbMin.z));

    parView[4] = vec4::MakeHomogeneousPositionVec4(obbMax);
    parView[5] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMax.y, obbMin.z));
    parView[6] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMax.y, obbMax.z));
    parView[7] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMax.y, obbMin.z));
}

void ExtractOBBCorners(const AABB3f& OBB, const mat4& Transform, MemoryView<vec4>& parView)
{
    ExtractAABBCorners(OBB, parView);
    foreachitem(corner, parView)
    {
        corner = Transform * corner;
    }
}

float ProjectBoxToAxis(const vec4& parAxis, const vec4& parCenter, MemoryView<vec4>& parView)
{
    float MaxAlongAxis = 0.f;
    foreachitemconst(corner, parView)
    {
        MaxAlongAxis = Max(fabs(Dot(corner - parCenter, parAxis)), MaxAlongAxis);
    }
    return MaxAlongAxis;
}

float ProjectBoxToAxis(const vec4& parAxis, const mat4& parTransform, const AABB3f& parBBox)
{
    const vec3 Max = parBBox.Max();
    return Max.x * fabs(Dot(parAxis, parTransform.Column(0))) + Max.y * fabs(Dot(parAxis, parTransform.Column(1))) + Max.z * fabs(Dot(parAxis, parTransform.Column(2)));
}

AABB3f ComputeAABBFromOBB(const AABB3f& OBB, const mat4& Transform)
{
    FixedSizedArrayInSitu<vec4, 8> Corners;

    auto CornersMemView = MemoryView<vec4>(Corners.data(), Corners.size());
    ExtractAABBCorners(OBB, CornersMemView);

    vec3 newMin(std::numeric_limits<float>::max()), newMax(-std::numeric_limits<float>::max());
    foreachitemconst(corner, Corners)
    {
        vec4 transformedCorner = Transform * corner;
        newMin = VecMin(newMin, transformedCorner.xyz());
        newMax = VecMax(newMax, transformedCorner.xyz());
    }

    return AABB3f(newMin, newMax);
}

bool FirstAABBContainsSecond(const AABB3f& parFirst, const AABB3f& parSecond)
{
    bool res = parFirst.Min().x <= parSecond.Min().x;
    res = res && parFirst.Min().y <= parSecond.Min().y;
    res = res && parFirst.Min().z <= parSecond.Min().z;

    res = res && parFirst.Max().x >= parSecond.Max().x;
    res = res && parFirst.Max().y >= parSecond.Max().y;
    res = res && parFirst.Max().z >= parSecond.Max().z;

    return res;
}
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine