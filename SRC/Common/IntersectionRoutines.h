#pragma once

namespace ECSEngine
{
class Frustum;

namespace Intersect
{
bool FrustumSphereIntersect(const Frustum& parFrustum, const glm::vec4& parSphere);
}
} // namespace ECSEngine
