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
    void Init(const vec3& parInitialPosition, const quat& parIntialOrientation, const float parInitialTime);
    void Update(const float parCurrentTime);

    void PushNewFullKeyframe(const vec3& parNewPosition, const quat& parNewOrientation, const float parTime);
    void PushNewPositionKeyframe(const vec3& parNewPosition, const float parTime);
    void PushNewRotationKeyframe(const quat& parNewOrientation, const float parTime);

    const vec3& Position() const { return FPosition.GetCurrentValue(); }
    const quat& Orientation() const { return FOrientation.GetCurrentValue(); }
    const mat4& LocalToWorld() const { return FLocalToWorld; }

private:
    ValueInterpolator<vec3> FPosition;
    ValueInterpolator<quat> FOrientation;
    mat4 FLocalToWorld = mat4::Identity();
};
} // namespace Rendering
} // namespace ECSEngine
