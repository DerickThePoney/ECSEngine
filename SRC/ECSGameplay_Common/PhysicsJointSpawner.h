#pragma once
#include "Common/Singleton.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/ModuleSystem.h"
#include "Physics/JointsConfig.h"
#include "Physics/PhysicsJointHandle.h"

namespace ECSEngine
{
using OnJointCreatedCallback = Delegate<void(const Physics::PhysicsJointHandle&)>;

class PhysicsJointSpawner : public Singleton<PhysicsJointSpawner>, public ModuleSystem
{
public:
    PhysicsJointSpawner();

    // Hackos
    void Finalize();
    static void Delete();

    void VirtualUpdate() override;

    void AddDistanceJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::DistanceJointConfig& Config, OnJointCreatedCallback* Callback = nullptr);
    void AddBallJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::BallJointConfig& Config, OnJointCreatedCallback* Callback = nullptr);
    void AddHingeJointToSpawn(const EntityId& EntityA, const EntityId& EntityB, const Physics::HingeJointConfig& Config, OnJointCreatedCallback* Callback = nullptr);

private:
    struct DistanceJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::DistanceJointConfig Config;
        OnJointCreatedCallback* Callback = nullptr;
    };
    std::vector<DistanceJointRecord> DistanceJointToSpawn;

    struct BallJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::BallJointConfig Config;
        OnJointCreatedCallback* Callback = nullptr;
    };
    std::vector<BallJointRecord> BallJointToSpawn;

    struct HingeJointRecord
    {
        EntityId A;
        EntityId B;
        Physics::HingeJointConfig Config;
        OnJointCreatedCallback* Callback = nullptr;
    };
    std::vector<HingeJointRecord> HingeJointToSpawn;
};
} // namespace ECSEngine