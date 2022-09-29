#include "stdafx.h"

#include "Carrier.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(Carrier);

void Carrier::Init(const vec3& parInitialPosition, const quat& parIntialOrientation, const float parInitialTime)
{
    FPosition.Init(parInitialPosition, parInitialTime);
    FOrientation.Init(parIntialOrientation, parInitialTime);
}

void Carrier::Update(const float parCurrentTime)
{
    FPosition.Update(parCurrentTime);
    FOrientation.Update(parCurrentTime);

    FLocalToWorld = Translation(FPosition.GetCurrentValue()) * (mat4)FOrientation.GetCurrentValue();
}

void Carrier::PushNewFullKeyframe(const vec3& parNewPosition, const quat& parNewOrientation, const float parTime)
{
    FPosition.AddNewKeyframe(parNewPosition, parTime);
    FOrientation.AddNewKeyframe(parNewOrientation, parTime);
}

void Carrier::PushNewPositionKeyframe(const vec3& parNewPosition, const float parTime)
{
    FPosition.AddNewKeyframe(parNewPosition, parTime);
}

void Carrier::PushNewRotationKeyframe(const quat& parNewOrientation, const float parTime)
{
    FOrientation.AddNewKeyframe(parNewOrientation, parTime);
}

} // namespace Rendering
} // namespace ECSEngine
