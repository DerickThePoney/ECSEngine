#include "stdafx.h"

#include "Polygon.h"

namespace ECSEngine
{

bool Polygon2D::IsClockWise() const
{
    float sum = 0;
    forrange(i, 0, FPoints.size())
    {
        const vec2 pi = FPoints[i];
        const vec2 pi_1 = FPoints[(i + 1) % FPoints.size()];
        sum += (pi_1.x - pi.x) * (pi_1.y + pi.y);
    }

    return sum > 0.f;
}

Polygon2D Polygon2D::Revert() const
{
    Polygon2D result;
    result.reserve(FPoints.size());
#ifdef PERFORM_SECURITY_CHECKS
    bool isClockwise = IsClockWise();
#endif //  PERFORM_SECURITY_CHECKS

    reverseforrange(i, 0, FPoints.size()) { result.push_back(FPoints[i]); }

    AlwaysCheckedAssert(result.IsClockWise() != isClockwise);
    return result;
}

float Polygon2D::Area2Signed() const
{
    AlwaysCheckedAssert(FPoints.size() > 2);
    float area = 0.f;

    const bool isClosed = FPoints.front() == FPoints.back();
    const u32 endIndex = (isClosed) ? FPoints.size() - 2 : FPoints.size() - 1;

    const vec2& pivotPoint = FPoints[0];
    u32 idx = 2;
    do
    {
        area += (FPoints[idx - 1].x - pivotPoint.x) * (FPoints[idx].y - pivotPoint.y) - (FPoints[idx].x - pivotPoint.x) * (FPoints[idx - 1].y - pivotPoint.y);
        idx++;
    } while (idx < endIndex);

    return area;
}

float Polygon2D::AreaSigned() const
{
    return 0.5f * Area2Signed();
}

float Polygon2D::Area2() const
{
    return abs(Area2Signed());
}

float Polygon2D::Area() const
{
    return 0.5f * Area2();
}

} // namespace ECSEngine
