#include "stdafx.h"

#include "IntersectionRoutines.h"

#include "Frustum.h"
#include "Triangle.h"

namespace ECSEngine
{
namespace Intersect
{
namespace
{
float sign(const glm::vec2 p1, const glm::vec2 p2, const glm::vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
} // namespace

bool PointTriangle2D(const Triangle2D& parTriangle, const glm::vec2 parPoint)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(parPoint, parTriangle.A, parTriangle.B);
    d2 = sign(parPoint, parTriangle.B, parTriangle.C);
    d3 = sign(parPoint, parTriangle.C, parTriangle.A);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
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
} // namespace Intersect
} // namespace ECSEngine
