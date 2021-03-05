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

bool PointInPolygon2D(const Polygon2D& parPolygon, const glm::vec2 parPoint, const bool parStrictlyInside /*= false*/)
{
    Ray2D ray(parPoint, glm::vec2(1.f, 0.f));
    std::vector<std::pair<bool, LinearComponentIntersection>> intersections;
    if (!RayPolygonIntersections2D(ray, parPolygon, intersections))
        return false;

    u32 nbIntersections = 0;
    forrange(i, 0, intersections.size())
    {
        if (intersections[i].first)
            nbIntersections += 1;
    }

    bool res = nbIntersections & 1;
    return res;
}

bool LinearComponentIntersection2D(const glm::vec2& parPointsDifference,
      const glm::vec2& parDirectionA,
      const glm::vec2& parDirectionB,
      LinearComponentIntersection& outIntersection)
{
    const glm::vec2 rayDirPerp = glm::vec2(-parDirectionA.y, parDirectionA.x);
    const glm::vec2 segDirPerp = glm::vec2(-parDirectionB.y, parDirectionB.x);

    const float denom = glm::dot(segDirPerp, parDirectionA);

    if (glm::abs(denom) < 0.01f)
        return false;

    const float aFactor = glm::dot(-segDirPerp, parPointsDifference) / denom;

    if (aFactor < 0.f)
        return false;

    const float bFactor = glm::dot(rayDirPerp, parPointsDifference) / (-denom);

    if (bFactor < 0.f)
        return false;

    outIntersection.Intersection1 = aFactor;
    outIntersection.Intersection2 = bFactor;

    return true;
}

bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, LinearComponentIntersection& outIntersection)
{
    const glm::vec2 w = parRay.FOrigin - parSegment.Start;
    const glm::vec2 segDirNormalised = parSegment.DirectionNormalized();

    if (glm::abs(glm::dot(segDirNormalised, parRay.FDirection)) > 0.99f)
    {
        return false;
    }

    const bool res = LinearComponentIntersection2D(w, parRay.FDirection, parSegment.Direction(), outIntersection);

    if (res && outIntersection.Intersection2 > 1.0f)
        return false;

    return res;
}

bool SegmentSegmentIntersection2D(const Segment2D& parSegment1, const Segment2D& parSegment2, LinearComponentIntersection& outIntersection)
{
    const glm::vec2 w = parSegment1.Start - parSegment2.Start;
    const glm::vec2 seg1DirNormalised = parSegment1.DirectionNormalized();
    const glm::vec2 seg2DirNormalised = parSegment2.DirectionNormalized();

    if (glm::abs(glm::dot(seg1DirNormalised, seg2DirNormalised)) > 0.99f)
    {
        return false;
    }

    const bool res = LinearComponentIntersection2D(w, parSegment1.Direction(), parSegment2.Direction(), outIntersection);

    if (res && (outIntersection.Intersection1 > 1.0f || outIntersection.Intersection2 > 1.0f))
        return false;

    return res;
}

bool SegmentPolygonIntersections2D_StopAtFirstIntersection(const Segment2D& parSegment,
      const Polygon2D& parPolygon,
      bool parDoNotConsiderSegmentEndPoints,
      bool parDoNotConsiderBorder)
{
    // TODO Add a check for goes through if segment in on endpoints
    const u32 polygonSize = (u32)parPolygon.size();
    forrange(i, 1, polygonSize)
    {
        Segment2D s = Segment2D(parPolygon[i - 1], parPolygon[i]);
        LinearComponentIntersection intersection;
        if (Intersection::SegmentSegmentIntersection2D(parSegment, s, intersection))
        {
            if (parDoNotConsiderSegmentEndPoints && (intersection.Intersection1 == 0.0f || intersection.Intersection1 == 1.0f) &&
                  (intersection.Intersection2 > 0.0f && intersection.Intersection2 < 1.0f))
                continue;
            if (parDoNotConsiderBorder && (intersection.Intersection1 == 1.0f || intersection.Intersection2 == 1.0f || intersection.Intersection2 == 0.0f))
                continue;
            return true;
        }
    }

    {
        Segment2D s = Segment2D(parPolygon[polygonSize - 1], parPolygon[0]);
        LinearComponentIntersection intersection;
        if (Intersection::SegmentSegmentIntersection2D(parSegment, s, intersection))
        {
            if (parDoNotConsiderSegmentEndPoints && (intersection.Intersection1 == 0.0f || intersection.Intersection1 == 1.0f))
                return false;
            if (parDoNotConsiderBorder && (intersection.Intersection1 == 1.0f || intersection.Intersection2 == 1.0f || intersection.Intersection2 == 0.0f))
                return false;
            return true;
        }
    }

    return false;
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

        if (outIntersection.Intersection1 >= inter.second.Intersection1)
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

bool RayPlaneIntersection3D(const Ray3D& parRay, const Plane& parPlane, float& outIntersection)
{
    const float rndotpn = glm::dot(parRay.FDirection, parPlane.Normal);
    if (std::abs(rndotpn) < 1e-3)
    {
        outIntersection = -1.f;
        return false;
    }

    outIntersection = glm::dot((parPlane.Position - parRay.FOrigin), parPlane.Normal) / rndotpn;
    return outIntersection >= 0.f;
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
