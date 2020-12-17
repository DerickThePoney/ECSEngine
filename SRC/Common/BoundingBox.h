#pragma once

namespace ECSEngine
{
template<typename VecType>
class BoundingBox
{
public:
    BoundingBox() { }
    BoundingBox(const VecType& parMin, const VecType& parMax)
        : FMin(parMin)
        , FMax(parMax)
    {
    }

    void SetMin(const VecType& parValue) { FMin = parValue; }
    void SetMax(const VecType& parValue) { FMax = parValue; }

    const VecType& Min() const { return FMin; }
    const VecType& Max() const { return FMax; }

    const VecType Center() const { return 0.5f * (FMin + FMax); }

    bool IsZero() const { return glm::length2(FMax - FMin) == 0.f; }

    SERIALIZE()
    {
        PROPERTYFIELD(Min, VecType(0));
        PROPERTYFIELD(Max, VecType(0));
    }

private:
    VecType FMin = VecType(0);
    VecType FMax = VecType(0);
};

using AABB3f = BoundingBox<glm::vec3>;
using AABB2f = BoundingBox<glm::vec2>;
} // namespace ECSEngine
