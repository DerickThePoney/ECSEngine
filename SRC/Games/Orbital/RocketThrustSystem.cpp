#include "stdafx.h"

#include "RocketThrustSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/RigidbodyModule.h"
#include "FuelTankModule.h"
#include "NozzleEngineModule.h"
#include "Physics/PhysicsAPI.h"
#include "RocketManagerModule.h"

namespace ECSEngine
{
RocketThrustSystem::RocketThrustSystem()
{
    RegisterDependency<RocketManagerModule>(EEntityWorlds::STANDARD);
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
    RegisterDependency<NozzleEngineModule>(EEntityWorlds::STANDARD);
    RegisterDependency<FuelTankModule>(EEntityWorlds::STANDARD);
}

void RocketThrustSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RocketManagerModule> rocketManagerAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<RigidbodyModule> rigidbodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<NozzleEngineModule> nozzleEngineAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<FuelTankModule> bodyTankAccessor(EEntityWorlds::STANDARD);

    AssertRelease(rocketManagerAccessor.size() <= 1); // Pour l'instant on en prend qu'un !!
    const float DT = TimeManager::GameplayDeltaTime();
    foreachitemconst(rocketManager, rocketManagerAccessor)
    {
        FuelTankModule* bodyTank = bodyTankAccessor[rocketManager.BodyId()];
        AssertRelease(bodyTank != nullptr);

        if (bodyTank->IsTankEmpty())
            continue;

        NozzleEngineModule* nozzleEngine = nozzleEngineAccessor[rocketManager.NozzleId()];
        AssertRelease(nozzleEngine != nullptr);
        nozzleEngine->SetThrottle(rocketManager.RocketThrottle());

        if (nozzleEngine->Throttle() <= 0.f)
        {
            continue;
        }

        RigidbodyModule* NozzleBody = rigidbodyAccessor[rocketManager.NozzleId()];
        AssertRelease(NozzleBody != nullptr);

        quat BodyOrientation;
        Physics::GetBodyOrientation(NozzleBody->BodyHandle(), BodyOrientation);

        // burn fuel and update body mass
        const float CurrentRemainingFuelMass = bodyTank->CurrentRemainingFuelMass();
        const float FuelFlow = nozzleEngine->FuelFlow() * DT;
        bodyTank->SetCurrentRemainingFuelMass(Max(0.f, CurrentRemainingFuelMass - FuelFlow));

        const float ConsumptionRatio = (FuelFlow > 1e-6f) ? Clamp01((CurrentRemainingFuelMass - bodyTank->CurrentRemainingFuelMass()) / FuelFlow) : 0.f;

        RigidbodyModule* BodyBody = rigidbodyAccessor[rocketManager.BodyId()];
        AssertRelease(BodyBody != nullptr);
        // TODO figure out a way of setting up the dry mass

        const float Prop = ConsumptionRatio * nozzleEngine->Throttle() * nozzleEngine->MaxThrust();
        vec3 PropulsionForce = (mat4(BodyOrientation) * vec4(0.f, Prop, 0.f, 0.f)).xyz();
        Physics::AddForceToBody(NozzleBody->BodyHandle(), PropulsionForce, false);
    }
}
} // namespace ECSEngine