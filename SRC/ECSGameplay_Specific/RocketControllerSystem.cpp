#include "stdafx.h"

#include "RocketControllerSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketControllerSystem::RocketControllerSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);

    FRight.FKeyboardKey = InputKeyNames::INPUT_KEY_RIGHT;
    FRight.FInputType = EInputType::REPEATED;

    FLeft.FKeyboardKey = InputKeyNames::INPUT_KEY_LEFT;
    FLeft.FInputType = EInputType::REPEATED;

    FUp.FKeyboardKey = InputKeyNames::INPUT_KEY_UP;
    FUp.FInputType = EInputType::REPEATED;

    FDown.FKeyboardKey = InputKeyNames::INPUT_KEY_DOWN;
    FDown.FInputType = EInputType::REPEATED;

    FFireEngines.FKeyboardKey = InputKeyNames::INPUT_KEY_SPACE;
    FFireEngines.FInputType = EInputType::REPEATED;
}

static constexpr float MaxSteeringAngle = 50.f;

void RocketControllerSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitem(rocketManager, rocketManagerAccessor)
    {
        // Handle joint orientation
        Physics::PhysicsJointHandle NozzleJointHandle = rocketManager.NozzleJointHandle();
        if (!NozzleJointHandle.IsValid())
        {
            continue;
        }

        vec3 TargetAngle{ 0.f };

        if (FUp.Evaluate())
            TargetAngle.z += MaxSteeringAngle;

        if (FDown.Evaluate())
            TargetAngle.z -= MaxSteeringAngle;

        if (FLeft.Evaluate())
            TargetAngle.x += MaxSteeringAngle;

        if (FRight.Evaluate())
            TargetAngle.x -= MaxSteeringAngle;

        Physics::SetBallTargetEulerAngles(NozzleJointHandle, true, TargetAngle, 15.f, 500.f);

        if (FFireEngines.Evaluate())
        {
            RigidbodyModule* NozzleBody = rigidbodyAccessor[rocketManager.NozzleId()];
            AssertRelease(NozzleBody != nullptr);
            quat BodyOrientation;
            Physics::GetBodyOrientation(NozzleBody->BodyHandle(), BodyOrientation);
            vec3 PropulsionForce = (mat4(BodyOrientation) * vec4(0.f, 50.f, 0.f, 0.f)).xyz();
            Physics::AddForceToBody(NozzleBody->BodyHandle(), PropulsionForce, true);
        }
    }
}
} // namespace ECSEngine
