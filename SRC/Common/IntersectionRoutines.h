#pragma once
#include "Ray.h"
#include "Segment.h"

namespace ECSEngine
{
class Frustum;
class Triangle2D;
class Polygon2D;

namespace Intersection
{
bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, float& outIntersection);
bool RayPolygonIntersections2D(const Ray2D& parRay, const Polygon2D& parPolygon, std::vector<std::pair<bool, float>>& outIntersections);
bool RayPolygonClosestIntersection2D(const Ray2D& parRay, const Polygon2D& parPolygon, const bool parDoNotConsiderRayOrigin, float& outIntersection);
bool PointTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint);
bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere);
} // namespace Intersection
} // namespace ECSEngine
