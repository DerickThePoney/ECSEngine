#pragma once
#include "Plane.h"
#include "Ray.h"
#include "Segment.h"

namespace ECSEngine
{
class Frustum;
class FrustumCorners;
class Triangle2D;
class Polygon2D;

template<typename T>
class BoundingBox;

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

bool PointInTriangle2D(const Triangle2D& parTriangle, const vec2 parPoint, const bool parStrictlyInside = false);
bool PointInPolygon2D(const Polygon2D& parPolygon, const vec2 parPoint, const bool parStrictlyInside = false);
bool FrustumSphereIntersect(const Frustum& parFrustum, const vec4& parSphere);
bool FrustumBoundingBoxIntersect(const Frustum& parFrustum, const FrustumCorners& parCorners, const BoundingBox<vec3>& parBox);

bool SphereBoundingBoxIntersect(const vec4& parSphere, const BoundingBox<vec3>& parBox);

// Rich slab test: returns entry/exit t and which face set the entry plane.
// Assumes ray direction is usable with 1/D (zeros → ±Inf, same as current Tavianator code).
// outHitAxis: 0=X, 1=Y, 2=Z, -1 if entry not from a forward face (e.g. origin inside).
// outHitSign: -1 = hit Min plane of that axis, +1 = hit Max plane.
bool RayAABBIntersection(const Ray3D& parRay, const BoundingBox<vec3>& parAABB, float parMaxDistance, float& outTMin, float& outTMax, i32& outHitAxis, float& outHitSign);

// Thin wrapper — same signature/behavior as today (used by AABBTree + unit tests).
bool RayAABBIntersection(const Ray3D& parRay, const BoundingBox<vec3>& parAABB);

bool AABBABBBIntersection(const BoundingBox<vec3>& parFirst, const BoundingBox<vec3>& parSecond);
} // namespace Intersection
} // namespace ECSEngine
