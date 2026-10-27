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
    void AddBallJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::BallJointConfig& Config);
    void AddHingeJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::HingeJointConfig& Config);

private:
    struct DistanceJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::DistanceJointConfig Config;
    };
    std::vector<DistanceJointRecord> DistanceJointToSpawn;

    struct BallJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::BallJointConfig Config;
    };
    std::vector<BallJointRecord> BallJointToSpawn;

    struct HingeJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::HingeJointConfig Config;
    };
    std::vector<HingeJointRecord> HingeJointToSpawn;
};
} // namespace ECSEngine