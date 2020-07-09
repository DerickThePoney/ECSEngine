#include "stdafx.h"

#include "IntersectionRoutines.h"

#include "Frustum.h"

namespace ECSEngine
{
namespace Intersect
{
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
} // namespace Intersect
} // namespace ECSEngine
