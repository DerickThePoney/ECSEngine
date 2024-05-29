#include "stdafx.h"

#include "PhysicsUpdateSystem.h"

#include "Common/InputManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "OrientationModule.h"
#include "Physics/PhysicsAPI.h"
#include "PositionModule.h"
#include "RigidbodyModule.h"

namespace ECSEngine
{

PhysicsUpdateSystem::PhysicsUpdateSystem()
    : parent_type()
{
    RegisterDepency<PositionModule>(EEntityWorlds::STANDARD);
    RegisterDepency<OrientationModule>(EEntityWorlds::STANDARD);
    RegisterDepency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

PhysicsUpdateSystem::~PhysicsUpdateSystem()
{
}

void PhysicsUpdateSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    Physics::UpdatePhysics();

    ModuleAccessor<RigidbodyModule> RigidBodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PositionModule> PositionAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<OrientationModule> OrientationAccessor(EEntityWorlds::STANDARD);

    foreachitemconst(rigidbody, RigidBodyAccessor)
    {
        const EntityId& UnitId = rigidbody.UnitId();

        PositionModule* positionModule = PositionAccessor[UnitId];
        if (positionModule == nullptr)
        {
            continue;
        }

        vec3 newPosition;
        if (Physics::GetBodyPosition(rigidbody.BodyHandle(), newPosition))
        {
            positionModule->SetPosition3D(newPosition);
        }
    }

    foreachitemconst(rigidbody, RigidBodyAccessor)
    {
        const EntityId& UnitId = rigidbody.UnitId();
        PositionModule* positionModule = PositionAccessor[UnitId];
        if (positionModule == nullptr)
        {
            continue;
        }

        if (positionModule->GetPosition3D().y < -20.f)
        {
            vec3 velocity;
            if (Physics::GetBodyVelocity(rigidbody.BodyHandle(), velocity))
            {
                Physics::AddImpulseToBody(rigidbody.BodyHandle(), vec3(0.f, -2.f * velocity.y, 0.f), true);
            }
        }

        if (Input::GetButtonDown(InputKeyNames::INPUT_KEY_LEFT_SHIFT))
        {
            Physics::AddForceToBody(rigidbody.BodyHandle(), vec3(0.f, 1000.f, 0.f), true);
        }
    }
}

} // namespace ECSEngine
