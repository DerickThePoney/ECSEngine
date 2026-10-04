#pragma once
#include "Common/BoundingBox.h"
#include "Common/MemoryView.h"

namespace ECSEngine
{
namespace Physics
{
namespace GeometryHelpers
{
void ExtractAABBCorners(const AABB3f& OBB, MemoryView<vec4>& parView);
void ExtractOBBCorners(const AABB3f& OBB, const mat4& Transform, MemoryView<vec4>& parView);
float ProjectBoxToAxis(const vec4& parAxis, const vec4& parCenter, MemoryView<vec4>& parView);
float ProjectBoxToAxis(const vec4& parAxis, const mat4& parTransform, const AABB3f& parBBox);
AABB3f ComputeAABBFromOBB(const AABB3f& OBB, const mat4& Transform);

bool FirstAABBContainsSecond(const AABB3f& parFirst, const AABB3f& parSecond);

// @Ericson - Real Time Collision Detection - p148-151
float ClosestPointSegmentSegment(const vec3& parAS, const vec3& parAE, float& outAT, vec3& outAC, const vec3& parBS, const vec3& parBE, float& outBT, vec3& outBC);

enum class ESegmentAABBFeature : u8
{
    Face,
    Edge,
    Vertex,
    Interior
};

struct SegmentAABBClosestResult
{
    vec3 OnSegment = vec3(0.f);
    vec3 OnAABB = vec3(0.f);
    float DistSq = 0.f;
    float ExitDepth = 0.f;
    bool Interior = false;
    ESegmentAABBFeature Feature = ESegmentAABBFeature::Vertex;
    u8 FaceAxis = 0;    // 0 = X, 1 = Y, 2 = Z
    float FaceSign = 1.f; // outward normal sign on FaceAxis
};

// Closest points between a segment and a solid AABB. When the segment intersects the
// volume, Interior is set and OnAABB/ExitDepth describe the minimum-face-exit MTD.
void ClosestPointsSegmentAABB(const vec3& parAS, const vec3& parAE, const vec3& parMin, const vec3& parMax, SegmentAABBClosestResult& outResult);
} // namespace GeometryHelpers
} // namespace Physics
} // namespace ECSEngine