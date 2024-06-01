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

void ShutdownPhysics()
{
    PhysicsEngine::Instance().Cleanup();
    PhysicsEngine::Destroy();
}

void UpdatePhysics()
{
    PhysicsEngine::Instance().UpdatePhysics(TimeManager::FrameDeltaTime());
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

bool AddForceToBody(const PhysicsBodyHandle& Handle, const vec3& Force, bool bTreatAsAcceleration)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddForceToBody(Handle, Force, bTreatAsAcceleration);
}

bool AddImpulseToBody(const PhysicsBodyHandle& Handle, const vec3& Impulse, bool bTreatAsVelocityChange)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.AddImpulseToBody(Handle, Impulse, bTreatAsVelocityChange);
}

} // namespace Physics
} // namespace ECSEngine
