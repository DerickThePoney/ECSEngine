#pragma once

namespace ECSEngine
{
namespace Physics
{
struct PhysicsBodyHandle;
struct PhysicsBodyConfig;

// Init and shutdown
void InitializePhysics(const std::string& ConfigurationFilename);
void ShutdownPhysics();

// Physics update
void UpdatePhysics(float parDeltaTime);
// TODO BACKWARD UPDATE

// Body creation
const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig);

// Body manipulation
bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position);
bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity);
bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration);
bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange);
} // namespace Physics
} // namespace ECSEngine