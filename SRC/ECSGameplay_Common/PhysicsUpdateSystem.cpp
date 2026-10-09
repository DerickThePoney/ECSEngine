#include "stdafx.h"

#include "PhysicsUpdateSystem.h"

#include "Common/InputCommands.h"
#include "Common/InputManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "OrientationModule.h"
#include "Physics/ContactEvent.h"
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

    // TODO Figure out a way to dispatch contact events
    auto ContactEvents = Physics::GetContactEvents();
    u32 ContactEventsCount = Physics::GetContactEventsCount();
    // - ContactEvents manager that registers FHandle -> EntityId maps and allows modules to register on to it.
    // - Messaging system ?
    // TODO STUFF WITH EVENTS

    ModuleAccessor<RigidbodyModule> RigidBodyAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<PositionModule> PositionAccessor(EEntityWorlds::STANDARD);
    ModuleAccessor<OrientationModule> OrientationAccessor(EEntityWorlds::STANDARD);

    foreachitemconst(rigidbody, RigidBodyAccessor)
    {
        const EntityId& UnitId = rigidbody.UnitId();

        {
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

        {
            OrientationModule* orientationModule = OrientationAccessor[UnitId];
            quat newOrientation;
            if (Physics::GetBodyOrientation(rigidbody.BodyHandle(), newOrientation))
            {
                orientationModule->SetOrientation(newOrientation);
            }
        }
    }
}

} // namespace ECSEngine
