#include "stdafx.h"

#include "AngleRange.h"

namespace ECSEngine
{
namespace AngleHelpers
{
float AngleDifference(const float parA, const float parB)
{
    float realB = parB;
    if (realB < parA)
    {
        static const float tau = 2.0f * Pi();
        realB += tau;
    }
    return realB - parA;
}
} // namespace AngleHelpers

AngleRange::AngleRange(std::pair<float, float> parRange)
    : FRange(parRange)
{
    if (parRange.first > parRange.second)
    {
        FRange = { FRange.second, FRange.first };
    }
}

bool AngleRange::Contains(const float parAngle) const
{
    AssertRelease(parAngle >= 0.f && parAngle <= 2.0f * Pi());
    static const float tau = 2.0f * Pi();
    if (FRange.first < 0.f && FRange.second >= 0.f)
    {
        if (parAngle >= FRange.first + tau)
            return true;

        if (parAngle <= FRange.second)
            return true;
    }
    else
    {
        if (parAngle >= FRange.first && parAngle <= FRange.second)
            return true;
    }
    return false;
}

} // namespace ECSEngine
