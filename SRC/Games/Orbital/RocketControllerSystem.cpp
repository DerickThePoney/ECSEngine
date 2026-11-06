#include "stdafx.h"

#include "RocketControllerSystem.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "NozzleEngineModule.h"
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

    FSASToggle.FKeyboardKey = InputKeyNames::INPUT_KEY_T;
    FSASToggle.FInputType = EInputType::PRESSED;

    FIncreaseThrottle.FKeyboardKey = InputKeyNames::INPUT_KEY_LEFT_SHIFT;
    FIncreaseThrottle.FInputType = EInputType::REPEATED;
    FDecreaseThrottle.FKeyboardKey = InputKeyNames::INPUT_KEY_LEFT_CONTROL;
    FDecreaseThrottle.FInputType = EInputType::REPEATED;

    FFullThrottle.FKeyboardKey = InputKeyNames::INPUT_KEY_W;
    FFullThrottle.FInputType = EInputType::PRESSED;
    FCutThrottle.FKeyboardKey = InputKeyNames::INPUT_KEY_X;
    FCutThrottle.FInputType = EInputType::PRESSED;
}

static constexpr float MaxSteeringAngle = Radians(50.f);

void RocketControllerSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!

    foreachitem(rocketManager, rocketManagerAccessor)
    {
        vec3 TargetAngle{ 0.f };
        bool bIsSteering = false;
        if (FUp.Evaluate())
        {
            TargetAngle.z += MaxSteeringAngle;
            bIsSteering = true;
        }

        if (FDown.Evaluate())
        {
            TargetAngle.z -= MaxSteeringAngle;
            bIsSteering = true;
        }

        if (FLeft.Evaluate())
        {
            TargetAngle.x += MaxSteeringAngle;
            bIsSteering = true;
        }

        if (FRight.Evaluate())
        {
            TargetAngle.x -= MaxSteeringAngle;
            bIsSteering = true;
        }

        rocketManager.SetTargetSteeringAngle(TargetAngle);
        rocketManager.SetIsSteering(bIsSteering);

        if (FFullThrottle.Evaluate())
            rocketManager.SetRocketThrottle(1.f);
        else if (FCutThrottle.Evaluate())
            rocketManager.SetRocketThrottle(0.f);
        else
        {
            float CurrentThrottle = rocketManager.RocketThrottle();
            if (FIncreaseThrottle.Evaluate())
                CurrentThrottle += 0.005f;
            else if (FDecreaseThrottle.Evaluate())
                CurrentThrottle -= 0.005f;

            rocketManager.SetRocketThrottle(CurrentThrottle);
        }

        bool bSASActivated = rocketManager.SASActivated();
        if (FSASToggle.Evaluate())
            bSASActivated = !bSASActivated;
        rocketManager.SetSASActivated(bSASActivated);
    }
}
} // namespace ECSEngine
