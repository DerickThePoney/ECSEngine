#pragma once

#include "Math/VectorTypes.h"

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
    VecType& Min() { return FMin; }
    VecType& Max() { return FMax; }

    const VecType Center() const { return 0.5f * (FMin + FMax); }
    const VecType Extent() const { return FMax - FMin; }

    bool IsZero() const { return length2(FMax - FMin) == 0.f; }

    void Inflate(const float parValue)
    {
        FMin -= parValue;
        FMax += parValue;
    }

    static BoundingBox Union(const BoundingBox& A, const BoundingBox& B)
    {
        BoundingBox res;
        res.SetMax(VecMax(A.Max(), B.Max()));
        res.SetMin(VecMin(A.Min(), B.Min()));
        return res;
    }

    SERIALIZE()
    {
        PROPERTYFIELD(Min, VecType(0));
        PROPERTYFIELD(Max, VecType(0));
    }

private:
    VecType FMin = VecType(0);
    VecType FMax = VecType(0);
};

using AABB3f = BoundingBox<vec3>;
using AABB2f = BoundingBox<vec2>;
} // namespace ECSEngine
