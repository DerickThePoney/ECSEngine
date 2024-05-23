#pragma once
#include "Common/Singleton.h"
#include "PhysicsEngineConfiguration.h"

namespace ECSEngine
{
struct PhysicsBodyHandle;

class PhysicsEngine : public Singleton<PhysicsEngine>
{
public:
    PhysicsEngine();
    ~PhysicsEngine();

    void Initialize(const PhysicsEngineConfiguration& PhysicsConfig);
    void Cleanup();

    const PhysicsEngineConfiguration& Config() const { return FConfig; }
    void SetConfig(const PhysicsEngineConfiguration& NewConfig) { FConfig = NewConfig; }

    const PhysicsBodyHandle CreateNewBody(/* ADD A CONFIG OBJECT */);

private:
    PhysicsEngineConfiguration FConfig;
};
} // namespace ECSEngine