#pragma once
#include "Common/PoolAllocator.h"
#include "Common/ValueInterpolator.h"

namespace ECSEngine
{
namespace Rendering
{
class Carrier
{
    DECLARE_POOL_ALLOCATED(Carrier);

public:
    Carrier() { }
    void Init(const glm::vec3& parInitialPosition, const glm::quat& parIntialOrientation, const float parInitialTime);
    void Update(const float parCurrentTime);

    void PushNewKeyframe(const glm::vec3& parNewPosition, const glm::quat& parNewOrientation, const float parTime);

    const glm::vec3& Position() const { return FPosition.GetCurrentValue(); }
    const glm::quat& Orientation() const { return FOrientation.GetCurrentValue(); }

private:
    ValueInterpolator<glm::vec3> FPosition;
    ValueInterpolator<glm::quat> FOrientation;
};
} // namespace Rendering
} // namespace ECSEngine
