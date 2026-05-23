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

// @Ericson - Real Time Collision Detection - p148-151
float ClosestPointSegmentSegment(const vec3& parAS, const vec3& parAE, float& outAT, vec3& outAC, const vec3& parBS, const vec3& parBE, float& outBT, vec3& outBC)
{
    vec3 DA = parAE - parAS;
    vec3 DB = parBE - parBS;
    vec3 R = parAS - parBS;

    float a = LengthSq(DA);
    float e = LengthSq(DB);

    constexpr float EPSILON = 1e-6f;
    if (a <= EPSILON && e <= EPSILON)
    {
        // degenerate case where both segments collapse to points
        outAT = outBT = 0.f;
        outAC = parAS;
        outBC = parBS;

        return LengthSq(outAC - outBC);
    }

    float f = Dot(DB, R);
    if (a <= EPSILON)
    {
        // first segment degenerates into a point
        outAT = 0.f;
        outBT = f / e;
        outBT = Clamp(outBT, 0.f, 1.f);
    }
    else
    {
        float c = Dot(DA, R);

        if (e <= EPSILON)
        {
            // second segment degenerates into a point
            outBT = 0.f;
            outAT = Clamp(-c / a, 0.f, 1.f);
        }
        else
        {
            float b = Dot(DA, DB);
            float denom = a * e - b * b;
            if (denom > EPSILON)
            {
                outAT = Clamp((b * f - c * e) / denom, 0.f, 1.f);
            }
            else
            {
                outAT = 0.f;
            }

            outBT = (b * outAT + f);
            if (outBT < 0.f)
            {
                outBT = 0.f;
                outAT = Clamp(-c / a, 0.f, 1.f);
            }
            else if (outBT > e)
            {
                outBT = 1.f;
                outAT = Clamp((b - c) / a, 0.f, 1.f);
            }
            else
            {
                outBT = outBT / e;
            }
        }
    }

    outAC = parAS + DA * outAT;
    outBC = parBS + DB * outBT;

    return Dot(outAC - outBC, outAC - outBC);
}
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine