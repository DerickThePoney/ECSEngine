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

bool PointInTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint, const bool parStrictlyInside /*= false*/)
{
    float d1, d2, d3;
    bool has_neg, has_pos, has_zeros;

    d1 = sign(parPoint, parTriangle.A, parTriangle.B);
    d2 = sign(parPoint, parTriangle.B, parTriangle.C);
    d3 = sign(parPoint, parTriangle.C, parTriangle.A);

    has_neg = (d1 < 0.f) || (d2 < 0.f) || (d3 < 0.f);
    has_pos = (d1 > 0.f) || (d2 > 0.f) || (d3 > 0.f);
    has_zeros = (d1 == 0.f) || (d2 == 0.f) || (d3 == 0.f);

    if (parStrictlyInside)
    {
        return !has_zeros && !(has_neg && has_pos);
    }
    else
    {
        return !(has_neg && has_pos);
    }
}

bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, LinearComponentIntersection& outIntersection)
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

    const float rayFactor = glm::dot(-segDirPerp, w) / denom;

    if (rayFactor < 0.f)
        return false;

    const float segmentFactor = glm::dot(rayDirPerp, w) / (-denom);

    if (segmentFactor < 0.f || segmentFactor > 1.f)
        return false;

    outIntersection.Intersection1 = rayFactor;
    outIntersection.Intersection2 = segmentFactor;

    return true;
}

bool RayPolygonIntersections2D(const Ray2D& parRay, const Polygon2D& parPolygon, std::vector<std::pair<bool, LinearComponentIntersection>>& outIntersections)
{
    bool result = false;
    const u32 polygonSize = (u32)parPolygon.size();
    outIntersections.resize(polygonSize, std::pair<bool, LinearComponentIntersection>{ false, LinearComponentIntersection() });

    forrange(i, 1, polygonSize)
    {
        Segment2D s = Segment2D(parPolygon[i - 1], parPolygon[i]);
        outIntersections[i - 1].first = Intersection::RaySegmentIntersection2D(parRay, s, outIntersections[i - 1].second);

        result = result || outIntersections[i - 1].first;
    }

    {
        Segment2D s = Segment2D(parPolygon[polygonSize - 1], parPolygon[0]);
        outIntersections[polygonSize - 1].first = Intersection::RaySegmentIntersection2D(parRay, s, outIntersections[polygonSize - 1].second);
        result = result || outIntersections[polygonSize - 1].first;
    }

    return result;
}

bool RayPolygonClosestIntersection2D(const Ray2D& parRay,
      const Polygon2D& parPolygon,
      const bool parDoNotConsiderRayOrigin,
      LinearComponentIntersection& outIntersection,
      u32& outClosestEdgeIndex)
{
    std::vector<std::pair<bool, LinearComponentIntersection>> intersections;
    if (!RayPolygonIntersections2D(parRay, parPolygon, intersections))
        return false;

    bool result = false;
    u32 closestEdgeIndex = -1;
    u32 currentEdge = 0;
    foreachitemconst(inter, intersections)
    {
        if (!inter.first)
        {
            currentEdge++;
            continue;
        }

        if (parDoNotConsiderRayOrigin && inter.second.Intersection1 == 0.f)
        {
            currentEdge++;
            continue;
        }

        if (outIntersection.Intersection1 > inter.second.Intersection1)
        {
            closestEdgeIndex = currentEdge;
            result = true;
            outIntersection = inter.second;
        }
        currentEdge++;
    }
    outClosestEdgeIndex = closestEdgeIndex;
    AlwaysCheckedAssert(parDoNotConsiderRayOrigin || (result == (outIntersection != LinearComponentIntersection())));
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
