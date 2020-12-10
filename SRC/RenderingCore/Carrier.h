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
    const glm::mat4& LocalToWorld() const { return FLocalToWorld; }

private:
    ValueInterpolator<glm::vec3> FPosition;
    ValueInterpolator<glm::quat> FOrientation;
    glm::mat4 FLocalToWorld = glm::identity<glm::mat4>();
};
} // namespace Rendering
} // namespace ECSEngine
