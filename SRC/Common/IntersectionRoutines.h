#pragma once
#include "Plane.h"
#include "Ray.h"
#include "Segment.h"

namespace ECSEngine
{
class Frustum;
class Triangle2D;
class Polygon2D;

namespace Intersection
{
struct LinearComponentIntersection
{
    float Intersection1 = std::numeric_limits<float>::max();
    float Intersection2 = std::numeric_limits<float>::max();

    bool operator==(const LinearComponentIntersection& parOther) const { return Intersection1 == parOther.Intersection1 && Intersection2 == parOther.Intersection2; }
    bool operator!=(const LinearComponentIntersection& parOther) const { return Intersection1 != parOther.Intersection1 || Intersection2 != parOther.Intersection2; }
};

bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, LinearComponentIntersection& outIntersection);
bool SegmentSegmentIntersection2D(const Segment2D& parSegment1, const Segment2D& parSegment2, LinearComponentIntersection& outIntersection);
bool SegmentPolygonIntersections2D_StopAtFirstIntersection(const Segment2D& parSegment,
      const Polygon2D& parPolygon,
      bool parDoNotConsiderSegmentEndPoints,
      bool parDoNotConsiderBorder);
bool RayPolygonIntersections2D(const Ray2D& parRay, const Polygon2D& parPolygon, std::vector<std::pair<bool, LinearComponentIntersection>>& outIntersections);
bool RayPolygonClosestIntersection2D(const Ray2D& parRay,
      const Polygon2D& parPolygon,
      const bool parDoNotConsiderRayOrigin,
      LinearComponentIntersection& outIntersection,
      u32& outClosestEdgeIndex);
bool RayPlaneIntersection3D(const Ray3D& parRay, const Plane& parPlane, float& outIntersection);

bool PointInTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint, const bool parStrictlyInside = false);
bool PointInPolygon2D(const Polygon2D& parPolygon, const glm::vec2 parPoint, const bool parStrictlyInside = false);
bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere);
} // namespace Intersection
} // namespace ECSEngine
