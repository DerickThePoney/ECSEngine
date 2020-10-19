#pragma once

namespace ECSEngine
{
class Frustum;
class Triangle2D;

namespace Intersect
{
bool PointTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint);
bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere);
} // namespace Intersect
} // namespace ECSEngine
