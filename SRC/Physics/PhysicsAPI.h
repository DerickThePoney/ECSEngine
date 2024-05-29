#pragma once

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle;
struct PhysicsBodyConfig;
struct PhysicsEngineConfiguration;

// Init and shutdown
void InitializePhysics(const std::string& ConfigurationFilename);
void SetConfig(const PhysicsEngineConfiguration& Config);
const PhysicsEngineConfiguration& GetConfig();
void ShutdownPhysics();

// Physics update
void UpdatePhysics();
// TODO BACKWARD UPDATE

// Body creation/destruction
const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig);
bool DestroyPhysicsBody(const PhysicsBodyHandle& Handle);

// Body manipulation
bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position);
bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity);

bool GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position);
bool GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity);

bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration);
bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange);
} // namespace Physics
} // namespace ECSEngine