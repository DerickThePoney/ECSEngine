#pragma once

namespace ECSEngine
{
namespace AngleHelpers
{
float AngleDifference(const float parA, const float parB);
}

class AngleRange
{
public:
    AngleRange(std::pair<float, float> parRange);

    bool Contains(const float parAngle) const;

    float Left() const { return FRange.first; }
    float Right() const { return FRange.second; }

private:
    std::pair<float, float> FRange;
};
} // namespace ECSEngine
