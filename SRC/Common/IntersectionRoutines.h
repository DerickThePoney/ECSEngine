#pragma once
#include "Ray.h"
#include "Segment.h"

namespace ECSEngine
{
class Frustum;
class Triangle2D;

namespace Intersection
{
bool RaySegmentIntersection2D(const Ray2D& parRay, const Segment2D& parSegment, float& outIntersection);
bool PointTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint);
bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere);
} // namespace Intersection
} // namespace ECSEngine
