#pragma once

namespace ECSEngine
{
class EnergyEfficiencyDescriptor
{
public:
    SERIALIZE() { PROPERTYFIELD(Values, std::vector<ThresholdEfficiencyPair>()); }

    float GetEfficiency(const float parRatio) const;
    void DrawEditor();

private:
    using ThresholdEfficiencyPair = std::pair<float, float>;
    std::vector<ThresholdEfficiencyPair> FValues;
};
} // namespace ECSEngine