#pragma once

namespace ECSEngine
{
class AngleRange
{
public:
    AngleRange(std::pair<float, float> parRange);

    bool Contains(const float parAngle) const;

private:
    std::pair<float, float> FRange;
};
} // namespace ECSEngine
