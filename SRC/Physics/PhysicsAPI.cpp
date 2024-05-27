#include "stdafx.h"

#include "PhysicsAPI.h"

#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
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

void ShutdownPhysics()
{
    PhysicsEngine::Instance().Cleanup();
    PhysicsEngine::Destroy();
}

void UpdatePhysics(float parDeltaTime)
{
    PhysicsEngine::Instance().UpdatePhysics(parDeltaTime);
}

const PhysicsBodyHandle CreateNewPhysicsBody(const mat4& Transform, const PhysicsBodyConfig& BodyConfig)
{
    PhysicsEngine& Engine = PhysicsEngine::Instance();
    return Engine.CreateNewPhysicsBody(Transform, BodyConfig);
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
