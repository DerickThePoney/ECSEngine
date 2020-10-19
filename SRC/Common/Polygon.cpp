#include "stdafx.h"

#include "Polygon.h"

namespace ECSEngine
{

bool Polygon2D::IsClockWise() const
{
    float sum = 0;
    forrange(i, 0, FPoints.size())
    {
        const glm::vec2 pi = FPoints[i];
        const glm::vec2 pi_1 = FPoints[(i + 1) % FPoints.size()];
        sum += (pi_1.x - pi.x) * (pi_1.y + pi.y);
    }

    return sum > 0.f;
}

Polygon2D Polygon2D::Revert() const
{
    Polygon2D result;
#ifdef PERFORM_SECURITY_CHECKS
    bool isClockwise = IsClockWise();
#endif //  PERFORM_SECURITY_CHECKS

    reverseforrange(i, 0, FPoints.size()) { result.push_back(FPoints[i]); }

    AlwaysCheckedAssert(result.IsClockWise() != isClockwise);
    return result;
}

} // namespace ECSEngine