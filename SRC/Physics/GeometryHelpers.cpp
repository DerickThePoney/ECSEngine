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
AABB3f ComputeAABBFromOBB(const AABB3f& OBB, const mat4& Transform)
{
    FixedSizedArrayInSitu<vec4, 8> Corners;
    const vec3& obbMin = OBB.Min();
    const vec3& obbMax = OBB.Max();
    Corners[0] = vec4::MakeHomogeneousPositionVec4(obbMin);
    Corners[1] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMin.y, obbMax.z));
    Corners[2] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMin.y, obbMax.z));
    Corners[3] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMin.y, obbMin.z));

    Corners[4] = vec4::MakeHomogeneousPositionVec4(obbMax);
    Corners[5] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMax.y, obbMin.z));
    Corners[6] = vec4::MakeHomogeneousPositionVec4(vec3(obbMin.x, obbMax.y, obbMax.z));
    Corners[7] = vec4::MakeHomogeneousPositionVec4(vec3(obbMax.x, obbMax.y, obbMin.z));

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

    res = res && parFirst.Max().x <= parSecond.Max().x;
    res = res && parFirst.Max().y <= parSecond.Max().y;
    res = res && parFirst.Max().z <= parSecond.Max().z;

    return res;
}
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine