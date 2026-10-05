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

namespace
{
void ClassifyClampedFeature(const vec3& parPoint, const vec3& parClamped, const vec3& parMin, const vec3& parMax, ESegmentAABBFeature& outFeature, u8& outFaceAxis, float& outFaceSign)
{
    constexpr float kEps = 1e-5f;
    u8 outsideCount = 0;
    u8 lastAxis = 0;
    float lastSign = 1.f;

    const float coords[3] = { parPoint.x, parPoint.y, parPoint.z };
    const float mins[3] = { parMin.x, parMin.y, parMin.z };
    const float maxs[3] = { parMax.x, parMax.y, parMax.z };
    const float clamped[3] = { parClamped.x, parClamped.y, parClamped.z };

    for (u8 axis = 0; axis < 3; ++axis)
    {
        if (coords[axis] < mins[axis] - kEps || coords[axis] > maxs[axis] + kEps || fabsf(coords[axis] - clamped[axis]) > kEps)
        {
            ++outsideCount;
            lastAxis = axis;
            lastSign = (clamped[axis] >= maxs[axis] - kEps) ? 1.f : -1.f;
        }
    }

    if (outsideCount >= 3)
        outFeature = ESegmentAABBFeature::Vertex;
    else if (outsideCount == 2)
        outFeature = ESegmentAABBFeature::Edge;
    else
    {
        outFeature = ESegmentAABBFeature::Face;
        outFaceAxis = lastAxis;
        outFaceSign = lastSign;
    }
}

int FeatureRank(ESegmentAABBFeature parFeature)
{
    // Prefer face over edge/vertex when distances tie so lying-on-face manifolds stay 2-point.
    switch (parFeature)
    {
    case ESegmentAABBFeature::Face:
    case ESegmentAABBFeature::Interior:
        return 0;
    case ESegmentAABBFeature::Edge:
        return 1;
    default:
        return 2;
    }
}

void ConsiderCandidate(const vec3& parOnSeg, const vec3& parOnBox, float parDistSq, ESegmentAABBFeature parFeature, u8 parFaceAxis, float parFaceSign, SegmentAABBClosestResult& outBest)
{
    constexpr float kTieEps = 1e-6f;
    const bool betterDist = parDistSq + kTieEps < outBest.DistSq;
    const bool tiedDist = fabsf(parDistSq - outBest.DistSq) <= kTieEps;
    const bool betterFeature = tiedDist && FeatureRank(parFeature) < FeatureRank(outBest.Feature);
    if (betterDist || betterFeature)
    {
        outBest.OnSegment = parOnSeg;
        outBest.OnAABB = parOnBox;
        outBest.DistSq = parDistSq;
        outBest.Feature = parFeature;
        outBest.FaceAxis = parFaceAxis;
        outBest.FaceSign = parFaceSign;
        outBest.Interior = false;
        outBest.ExitDepth = 0.f;
    }
}

bool SegmentIntersectsAABB(const vec3& parAS, const vec3& parAE, const vec3& parMin, const vec3& parMax, vec3& outInteriorPoint)
{
    const vec3 Delta = parAE - parAS;
    float tEnter = 0.f;
    float tExit = 1.f;

    const float start[3] = { parAS.x, parAS.y, parAS.z };
    const float delta[3] = { Delta.x, Delta.y, Delta.z };
    const float mins[3] = { parMin.x, parMin.y, parMin.z };
    const float maxs[3] = { parMax.x, parMax.y, parMax.z };

    constexpr float kEps = 1e-8f;
    for (u8 axis = 0; axis < 3; ++axis)
    {
        if (fabsf(delta[axis]) <= kEps)
        {
            if (start[axis] < mins[axis] || start[axis] > maxs[axis])
                return false;
            continue;
        }

        float t1 = (mins[axis] - start[axis]) / delta[axis];
        float t2 = (maxs[axis] - start[axis]) / delta[axis];
        if (t1 > t2)
        {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
        }

        tEnter = Max(tEnter, t1);
        tExit = Min(tExit, t2);
        if (tEnter > tExit)
            return false;
    }

    const float tInterior = Clamp(0.5f * (tEnter + tExit), 0.f, 1.f);
    outInteriorPoint = parAS + Delta * tInterior;
    return true;
}

void ComputeMinFaceExit(const vec3& parPoint, const vec3& parMin, const vec3& parMax, vec3& outSurfacePoint, float& outExitDepth, u8& outFaceAxis, float& outFaceSign)
{
    const float distances[6] = {
        parPoint.x - parMin.x,
        parMax.x - parPoint.x,
        parPoint.y - parMin.y,
        parMax.y - parPoint.y,
        parPoint.z - parMin.z,
        parMax.z - parPoint.z,
    };

    int bestFace = 0;
    for (int i = 1; i < 6; ++i)
    {
        if (distances[i] < distances[bestFace])
            bestFace = i;
    }

    outSurfacePoint = parPoint;
    outExitDepth = distances[bestFace];
    switch (bestFace)
    {
    case 0:
        outSurfacePoint.x = parMin.x;
        outFaceAxis = 0;
        outFaceSign = -1.f;
        break;
    case 1:
        outSurfacePoint.x = parMax.x;
        outFaceAxis = 0;
        outFaceSign = 1.f;
        break;
    case 2:
        outSurfacePoint.y = parMin.y;
        outFaceAxis = 1;
        outFaceSign = -1.f;
        break;
    case 3:
        outSurfacePoint.y = parMax.y;
        outFaceAxis = 1;
        outFaceSign = 1.f;
        break;
    case 4:
        outSurfacePoint.z = parMin.z;
        outFaceAxis = 2;
        outFaceSign = -1.f;
        break;
    default:
        outSurfacePoint.z = parMax.z;
        outFaceAxis = 2;
        outFaceSign = 1.f;
        break;
    }
}

void ConsiderFaceRectangle(const vec3& parAS, const vec3& parAE, u8 parAxis, float parPlane, float parUMin, float parUMax, float parVMin, float parVMax, float parFaceSign,
      SegmentAABBClosestResult& outBest)
{
    const u8 uAxis = (parAxis + 1) % 3;
    const u8 vAxis = (parAxis + 2) % 3;

    const float start[3] = { parAS.x, parAS.y, parAS.z };
    const float end[3] = { parAE.x, parAE.y, parAE.z };
    const float deltaAxis = end[parAxis] - start[parAxis];

    // Orthogonal / parallel hit against the face plane, kept only if projection lands in the face rect.
    constexpr float kEps = 1e-8f;
    if (fabsf(deltaAxis) > kEps)
    {
        const float t = (parPlane - start[parAxis]) / deltaAxis;
        if (t >= 0.f && t <= 1.f)
        {
            const vec3 onSeg = parAS + (parAE - parAS) * t;
            const float onSegCoords[3] = { onSeg.x, onSeg.y, onSeg.z };
            if (onSegCoords[uAxis] >= parUMin - kEps && onSegCoords[uAxis] <= parUMax + kEps && onSegCoords[vAxis] >= parVMin - kEps && onSegCoords[vAxis] <= parVMax + kEps)
            {
                vec3 onBox = onSeg;
                float boxCoords[3] = { onBox.x, onBox.y, onBox.z };
                boxCoords[parAxis] = parPlane;
                onBox = vec3(boxCoords[0], boxCoords[1], boxCoords[2]);
                ConsiderCandidate(onSeg, onBox, LengthSq(onSeg - onBox), ESegmentAABBFeature::Face, parAxis, parFaceSign, outBest);
            }
        }
    }
    else
    {
        // Segment parallel to the face: any overlapping UV span shares the same plane distance.
        const float exteriorDistance = parFaceSign * (start[parAxis] - parPlane);
        if (exteriorDistance >= -kEps)
        {
            float tMin = 0.f;
            float tMax = 1.f;
            const float delta[3] = { end[0] - start[0], end[1] - start[1], end[2] - start[2] };

            auto Clip = [&](u8 parClipAxis, float parMin, float parMax) {
                const float s = start[parClipAxis];
                const float d = delta[parClipAxis];
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

            Clip(uAxis, parUMin, parUMax);
            Clip(vAxis, parVMin, parVMax);

            if (tMin <= tMax)
            {
                const float tMid = 0.5f * (tMin + tMax);
                const vec3 onSeg = parAS + (parAE - parAS) * tMid;
                vec3 onBox = onSeg;
                float boxCoords[3] = { onBox.x, onBox.y, onBox.z };
                boxCoords[parAxis] = parPlane;
                onBox = vec3(boxCoords[0], boxCoords[1], boxCoords[2]);
                ConsiderCandidate(onSeg, onBox, exteriorDistance * exteriorDistance, ESegmentAABBFeature::Face, parAxis, parFaceSign, outBest);
            }
        }
    }

    // Four face edges (covers projection outside the rectangle and grazing cases).
    auto MakePoint = [&](float a, float u, float v) {
        float c[3];
        c[parAxis] = a;
        c[uAxis] = u;
        c[vAxis] = v;
        return vec3(c[0], c[1], c[2]);
    };

    const vec3 edges[4][2] = {
        { MakePoint(parPlane, parUMin, parVMin), MakePoint(parPlane, parUMax, parVMin) },
        { MakePoint(parPlane, parUMin, parVMax), MakePoint(parPlane, parUMax, parVMax) },
        { MakePoint(parPlane, parUMin, parVMin), MakePoint(parPlane, parUMin, parVMax) },
        { MakePoint(parPlane, parUMax, parVMin), MakePoint(parPlane, parUMax, parVMax) },
    };

    for (const auto& edge : edges)
    {
        float tA, tB;
        vec3 onSeg, onEdge;
        const float distSq = ClosestPointSegmentSegment(parAS, parAE, tA, onSeg, edge[0], edge[1], tB, onEdge);

        constexpr float kEdgeEps = 1e-4f;
        const bool atVertex = (tB <= kEdgeEps || tB >= 1.f - kEdgeEps);
        const ESegmentAABBFeature feature = atVertex ? ESegmentAABBFeature::Vertex : ESegmentAABBFeature::Edge;
        ConsiderCandidate(onSeg, onEdge, distSq, feature, parAxis, parFaceSign, outBest);
    }
}
} // namespace

void ClosestPointsSegmentAABB(const vec3& parAS, const vec3& parAE, const vec3& parMin, const vec3& parMax, SegmentAABBClosestResult& outResult)
{
    outResult = SegmentAABBClosestResult{};
    outResult.DistSq = std::numeric_limits<float>::max();

    vec3 interiorPoint;
    if (SegmentIntersectsAABB(parAS, parAE, parMin, parMax, interiorPoint))
    {
        vec3 surfacePoint;
        float exitDepth = 0.f;
        u8 faceAxis = 0;
        float faceSign = 1.f;
        ComputeMinFaceExit(interiorPoint, parMin, parMax, surfacePoint, exitDepth, faceAxis, faceSign);

        outResult.OnSegment = interiorPoint;
        outResult.OnAABB = surfacePoint;
        outResult.DistSq = 0.f;
        outResult.ExitDepth = exitDepth;
        outResult.Interior = true;
        outResult.Feature = ESegmentAABBFeature::Interior;
        outResult.FaceAxis = faceAxis;
        outResult.FaceSign = faceSign;
        return;
    }

    // Endpoint clamps (covers outside corners/faces when the closest point is an endpoint).
    const vec3 endpoints[2] = { parAS, parAE };
    for (const vec3& endpoint : endpoints)
    {
        const vec3 clamped = Clamp(endpoint, parMin, parMax);
        const float distSq = LengthSq(endpoint - clamped);

        // Clamp identity ⇒ endpoint is inside the solid; treat as interior MTD.
        if (distSq <= 1e-12f)
        {
            vec3 surfacePoint;
            float exitDepth = 0.f;
            u8 faceAxis = 0;
            float faceSign = 1.f;
            ComputeMinFaceExit(endpoint, parMin, parMax, surfacePoint, exitDepth, faceAxis, faceSign);

            outResult.OnSegment = endpoint;
            outResult.OnAABB = surfacePoint;
            outResult.DistSq = 0.f;
            outResult.ExitDepth = exitDepth;
            outResult.Interior = true;
            outResult.Feature = ESegmentAABBFeature::Interior;
            outResult.FaceAxis = faceAxis;
            outResult.FaceSign = faceSign;
            return;
        }

        ESegmentAABBFeature feature = ESegmentAABBFeature::Vertex;
        u8 faceAxis = 0;
        float faceSign = 1.f;
        ClassifyClampedFeature(endpoint, clamped, parMin, parMax, feature, faceAxis, faceSign);
        ConsiderCandidate(endpoint, clamped, distSq, feature, faceAxis, faceSign, outResult);
    }

    // Six face rectangles.
    ConsiderFaceRectangle(parAS, parAE, 0, parMin.x, parMin.y, parMax.y, parMin.z, parMax.z, -1.f, outResult);
    ConsiderFaceRectangle(parAS, parAE, 0, parMax.x, parMin.y, parMax.y, parMin.z, parMax.z, 1.f, outResult);
    ConsiderFaceRectangle(parAS, parAE, 1, parMin.y, parMin.z, parMax.z, parMin.x, parMax.x, -1.f, outResult);
    ConsiderFaceRectangle(parAS, parAE, 1, parMax.y, parMin.z, parMax.z, parMin.x, parMax.x, 1.f, outResult);
    ConsiderFaceRectangle(parAS, parAE, 2, parMin.z, parMin.x, parMax.x, parMin.y, parMax.y, -1.f, outResult);
    ConsiderFaceRectangle(parAS, parAE, 2, parMax.z, parMin.x, parMax.x, parMin.y, parMax.y, 1.f, outResult);
}
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine