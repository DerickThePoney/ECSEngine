#pragma once
#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleSystem.h"
#include "Physics/JointsConfig.h"

namespace ECSEngine
{
class PhysicsJointSpawner : public Singleton<PhysicsJointSpawner>, public ModuleSystem
{
public:
    PhysicsJointSpawner();

    // Hackos
    void Finalize();
    static void Delete();

    void VirtualUpdate() override;

    void AddDistanceJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::DistanceJointConfig& Config);

private:
    struct DistanceJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::DistanceJointConfig Config;
    };
    std::vector<DistanceJointRecord> DistanceJointToSpawn;
};
} // namespace ECSEngine