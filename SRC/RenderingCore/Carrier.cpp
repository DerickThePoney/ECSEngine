#include "stdafx.h"

#include "Carrier.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(Carrier);

void Carrier::Init(const glm::vec3& parInitialPosition, const glm::quat& parIntialOrientation, const float parInitialTime)
{
    FPosition.Init(parInitialPosition, parInitialTime);
    FOrientation.Init(parIntialOrientation, parInitialTime);
}

void Carrier::Update(const float parCurrentTime)
{
    FPosition.Update(parCurrentTime);
    FOrientation.Update(parCurrentTime);

    FLocalToWorld = glm::translate(FPosition.GetCurrentValue()) * (glm::mat4)FOrientation.GetCurrentValue();
}

void Carrier::PushNewKeyframe(const glm::vec3& parNewPosition, const glm::quat& parNewOrientation, const float parTime)
{
    FPosition.AddNewKeyframe(parNewPosition, parTime);
    FOrientation.AddNewKeyframe(parNewOrientation, parTime);
}

} // namespace Rendering
} // namespace ECSEngine