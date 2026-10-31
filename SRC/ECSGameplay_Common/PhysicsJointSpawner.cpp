#include "stdafx.h"

#include "PhysicsJointSpawner.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "Physics/PhysicsAPI.h"
#include "Physics/PhysicsBodyHandle.h"
#include "Physics/PhysicsJointHandle.h"
#include "RigidbodyModule.h"

namespace ECSEngine
{
PhysicsJointSpawner::PhysicsJointSpawner()
{
    RegisterDependency<RigidbodyModule>(EEntityWorlds::STANDARD);
}

void PhysicsJointSpawner::Finalize()
{
    ModuleSystem::Destroy();
}

void PhysicsJointSpawner::Delete()
{
    Singleton<PhysicsJointSpawner>::Destroy();
}

void PhysicsJointSpawner::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<RigidbodyModule> rigidBodies(EEntityWorlds::STANDARD);

    foreachitemconst(record, DistanceJointToSpawn)
    {
        RigidbodyModule* EntityABodyModule = rigidBodies[record.A];
        if (EntityABodyModule == nullptr)
            continue;

        RigidbodyModule* EntityBBodyModule = rigidBodies[record.B];
        if (EntityBBodyModule == nullptr)
            continue;

        Physics::PhysicsJointHandle Handle = Physics::CreateDistanceJoint(EntityABodyModule->BodyHandle(), EntityBBodyModule->BodyHandle(), record.Config);
        if (record.Callback != nullptr)
        {
            (*record.Callback)(Handle);
        }
    }

    DistanceJointToSpawn.clear();

    foreachitemconst(record, BallJointToSpawn)
    {
        RigidbodyModule* EntityABodyModule = rigidBodies[record.A];
        if (EntityABodyModule == nullptr)
            continue;

        RigidbodyModule* EntityBBodyModule = rigidBodies[record.B];
        if (EntityBBodyModule == nullptr)
            continue;

        Physics::PhysicsJointHandle Handle = Physics::CreateBallJoint(EntityABodyModule->BodyHandle(), EntityBBodyModule->BodyHandle(), record.Config);
        if (record.Callback != nullptr)
        {
            (*record.Callback)(Handle);
        }
    }

    BallJointToSpawn.clear();

    foreachitemconst(record, HingeJointToSpawn)
    {
        RigidbodyModule* EntityABodyModule = rigidBodies[record.A];
        if (EntityABodyModule == nullptr)
            continue;

        RigidbodyModule* EntityBBodyModule = rigidBodies[record.B];
        if (EntityBBodyModule == nullptr)
            continue;

        Physics::PhysicsJointHandle Handle = Physics::CreateHingeJoint(EntityABodyModule->BodyHandle(), EntityBBodyModule->BodyHandle(), record.Config);
        if (record.Callback != nullptr)
        {
            (*record.Callback)(Handle);
        }
    }

    HingeJointToSpawn.clear();
}

void PhysicsJointSpawner::AddDistanceJointToSpawn(const EntityId& EntityA,
      const EntityId& EntityB,
      const Physics::DistanceJointConfig& Config,
      OnJointCreatedCallback* Callback /*= nullptr*/)
{
    DistanceJointRecord record{ .A = EntityA, .B = EntityB, .Config = Config, .Callback = Callback };
    DistanceJointToSpawn.emplace_back(record);
}

void PhysicsJointSpawner::AddBallJointToSpawn(const EntityId& EntityA,
      const EntityId& EntityB,
      const Physics::BallJointConfig& Config,
      OnJointCreatedCallback* Callback /*= nullptr*/)
{
    BallJointRecord record{ .A = EntityA, .B = EntityB, .Config = Config, .Callback = Callback };
    BallJointToSpawn.emplace_back(record);
}

void PhysicsJointSpawner::AddHingeJointToSpawn(const EntityId& EntityA,
      const EntityId& EntityB,
      const Physics::HingeJointConfig& Config,
      OnJointCreatedCallback* Callback /*= nullptr*/)
{
    HingeJointRecord record{ .A = EntityA, .B = EntityB, .Config = Config, .Callback = Callback };
    HingeJointToSpawn.emplace_back(record);
}
} // namespace ECSEngine
