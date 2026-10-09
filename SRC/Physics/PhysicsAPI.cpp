#include "stdafx.h"

#include "PhysicsAPI.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "PhysicsBodyConfig.h"
#include "PhysicsBodyHandle.h"
#include "PhysicsEngine.h"

#include <fstream>

namespace ECSEngine
{
namespace Physics
{

void InitializePhysics(const std::string& ConfigurationFilename)
{
    PhysicsEngine::CreateIFP();

    PhysicsEngineConfiguration config;
    {
        AssertRelease(PhysicsEngine::HasInstance());
        Resource r(ConfigurationFilename);
        if (GlobalResourceCache::Instance().FCache->FileExists(&r))
        {
            auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
            AssertRelease(handle != nullptr);
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream istr(&buff, std::istream::in);
            cereal::JSONInputArchive archive(istr);

            archive(NAMEDPROPERTY("PhysicsEngineConfiguration", config));
        }
        else
        {
            std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + ConfigurationFilename);
            cereal::JSONOutputArchive archive(ofstr);
            archive(NAMEDPROPERTY("PhysicsEngineConfiguration", config));
        }
    }
    PhysicsEngine::Instance().Initialize(config);
}

void SetConfig(const PhysicsEngineConfiguration& Config)
{
    PhysicsEngine::Instance().SetConfig(Config);
}

const PhysicsEngineConfiguration& GetConfig()
{
    return PhysicsEngine::Instance().Config();
}

PhysicsCollisionPresetManager& GetCollisionPresetManager()
{
    return PhysicsEngine::Instance().CollisionPresetManager();
}

void ShutdownPhysics()
{
    PhysicsEngine::Instance().Cleanup();
    PhysicsEngine::Destroy();
}

void UpdatePhysics()
{
    PhysicsEngine::Instance().UpdatePhysics(TimeManager::GameplayDeltaTime());
}

const std::array<ContactEvent, 1024>& GetContactEvents()
{
    return PhysicsEngine::Instance().GetContactEvents();
}

u32 GetContactEventsCount()
{
    return PhysicsEngine::Instance().GetContactEventsCount();
}

const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.CreateNewPhysicsBody(Transform, BodyConfig);
}

bool DestroyPhysicsBody(const PhysicsBodyHandle& Handle)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.DestroyPhysicsBody(Handle);
}

#pragma region GettersAndSetterForBodies
bool SetBodyPosition(const PhysicsBodyHandle& Handle, const vec3& Position)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.SetBodyPosition(Handle, Position);
}

bool SetBodyVelocity(const PhysicsBodyHandle& Handle, const vec3& Velocity)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.SetBodyVelocity(Handle, Velocity);
}

bool SetBodyOrientation(const PhysicsBodyHandle& Handle, const quat& Orientation)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.SetBodyOrientation(Handle, Orientation);
}

bool SetBodyRotationVelocity(const PhysicsBodyHandle& Handle, const vec3& RotationVelocity)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.SetBodyRotationVelocity(Handle, RotationVelocity);
}

bool GetBodyPosition(const PhysicsBodyHandle& Handle, vec3& Position)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.GetBodyPosition(Handle, Position);
}

bool GetBodyVelocity(const PhysicsBodyHandle& Handle, vec3& Velocity)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.GetBodyVelocity(Handle, Velocity);
}

bool GetBodyOrientation(const PhysicsBodyHandle& Handle, quat& Orientation)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.GetBodyOrientation(Handle, Orientation);
}

bool GetBodyRotationVelocity(const PhysicsBodyHandle& Handle, vec3& RotationVelocity)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.GetBodyRotationVelocity(Handle, RotationVelocity);
}

#pragma endregion GettersAndSetterForBodies

#pragma region ForcesTorquesAndImpulses
bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddForceToBody(Handle, Force, bTreatAsAcceleration);
}

bool AddForceAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Force, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsAcceleration)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddForceAtPointToBody(Handle, Force, Point, bTreatPointAsLocalCoord, bTreatAsAcceleration);
}

bool AddTorqueToBody(const PhysicsBodyHandle& Handle, const vec3& Torque)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddTorqueToBody(Handle, Torque);
}

bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddImpulseToBody(Handle, Impulse, bTreatAsVelocityChange);
}

bool AddImpulseAtPointToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, const vec3& Point, bool bTreatPointAsLocalCoord, bool bTreatAsVelocityChange)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddImpulseAtPointToBody(Handle, Impulse, Point, bTreatPointAsLocalCoord, bTreatAsVelocityChange);
}

bool AddRotationImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& RotationImpulse, bool bTreatAsRotationVelocityChange)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddRotationImpulseToBody(Handle, RotationImpulse, bTreatAsRotationVelocityChange);
}

#pragma endregion ForcesTorquesAndImpulses
#ifdef WITH_VISUAL_DEBUG
void DrawDebugs()
{
    PhysicsEngine::Instance().DrawDebug();
}
#endif

} // namespace Physics
} // namespace ECSEngine
