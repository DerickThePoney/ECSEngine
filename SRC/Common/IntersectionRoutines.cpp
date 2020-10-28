#include "stdafx.h"

#include "IntersectionRoutines.h"

#include "Frustum.h"
#include "Polygon.h"
#include "Triangle.h"

namespace ECSEngine
{
namespace Intersection
{
namespace
{
float sign(const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
} // namespace

bool PointTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(parPoint, parTriangle.A, parTriangle.B);
    d2 = sign(parPoint, parTriangle.B, parTriangle.C);
    d3 = sign(parPoint, parTriangle.C, parTriangle.A);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, float& outIntersection)
{
    const glm::vec2 w = parRay.FOrigin - parSegment.Start;
    const glm::vec2 segDirNormalised = parSegment.DirectionNormalized();

    if (glm::abs(glm::dot(segDirNormalised, parRay.FDirection)) > 0.99f)
    {
        return false;
    }
    const glm::vec2 segDir = parSegment.Direction();
    const glm::vec2 rayDirPerp = glm::vec2(-parRay.FDirection.y, parRay.FDirection.x);
    const glm::vec2 segDirPerp = glm::vec2(-segDir.y, segDir.x);

    const float denom = glm::dot(segDirPerp, parRay.FDirection);

    if (glm::abs(denom) < 0.01f)
        return false;

    const float t1 = glm::dot(-segDirPerp, w) / denom;

    if (t1 < 0.f)
        return false;

    const float s1 = glm::dot(rayDirPerp, w) / (-denom);

    if (s1 < 0.f || s1 > 1.f)
        return false;

    outIntersection = t1;

    return true;
}

bool RayPolygonIntersections2D(const Ray2D& parRay, const Polygon2D& parPolygon, std::vector<std::pair<bool, float>>& outIntersections)
{
    bool result = false;
    const u32 polygonSize = (u32)parPolygon.size();
    Ray2D r = Ray2D(parPolygon[polygonSize - 1], glm::vec2(1.f, 0.f));
    outIntersections.resize(polygonSize, std::pair<bool, float>{ false, -1.f });

    forrange(i, 1, polygonSize)
    {
        Segment2D s = Segment2D(parPolygon[i - 1], parPolygon[i]);
        outIntersections[i - 1].first = Intersection::RaySegmentIntersection2D(r, s, outIntersections[i - 1].second);

        result = result || outIntersections[i - 1].first;
    }

    {
        Segment2D s = Segment2D(parPolygon[polygonSize - 1], parPolygon[0]);
        outIntersections[polygonSize - 1].first = Intersection::RaySegmentIntersection2D(r, s, outIntersections[polygonSize - 1].second);
        result = result || outIntersections[polygonSize - 1].first;
    }

    return result;
}

bool RayPolygonClosestIntersection2D(const Ray2D& parRay, const Polygon2D& parPolygon, const bool parDoNotConsiderRayOrigin, float& outIntersection)
{
    outIntersection = std::numeric_limits<float>::max();
    std::vector<std::pair<bool, float>> intersections;
    if (!RayPolygonIntersections2D(parRay, parPolygon, intersections))
        return false;

    bool result = false;
    foreachitemconst(inter, intersections)
    {
        if (!inter.first)
            continue;

        if (parDoNotConsiderRayOrigin && inter.second == 0.f)
            continue;

        if (outIntersection > inter.second)
        {
            result = true;
            outIntersection = inter.second;
        }
    }

    AlwaysCheckedAssert(parDoNotConsiderRayOrigin || (result == (outIntersection != std::numeric_limits<float>::max())));
    return result;
}

bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere)
{
    MemoryView<const glm::vec4> frustumPlane = parFrustum.GetPlanes();

    foreachitem(plane, frustumPlane)
    {
        const float dist = glm::dot(plane, glm::vec4(glm::xyz(parSphere), 1.0f));
        if (dist < -parSphere.w)
            return false;
    }

    return true;
}
} // namespace Intersection
} // namespace ECSEngine
