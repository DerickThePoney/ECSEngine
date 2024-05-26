#pragma once
#include "Common/IdGenerator.h"
#include "Common/Singleton.h"
#include "PhysicsEngineConfiguration.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle;
struct PhysicsBodyConfig;

class PhysicsEngine : public Singleton<PhysicsEngine>
{
public:
    PhysicsEngine();
    ~PhysicsEngine();

    void Initialize(const PhysicsEngineConfiguration& PhysicsConfig);
    void Cleanup();

    void UpdatePhysics(float parDeltaTime);

    const PhysicsEngineConfiguration& Config() const { return FConfig; }
    void SetConfig(const PhysicsEngineConfiguration& NewConfig) { FConfig = NewConfig; }

    const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig);

    // body manipulation
    bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position);
    bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity);
    bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration);
    bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange);

private:
    PhysicsEngineConfiguration FConfig;

    IdGenerator FHandleGenerator; // TODO MAKE PHYSICS HANDLE ID GENERATOR THAT SPECIALISES THIS
    std::vector<RigidBody> FRigidbodies;
};
} // namespace Physics
} // namespace ECSEngine