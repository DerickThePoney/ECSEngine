
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "Physics/PhysicsBodyConfig.h"
#include "Physics/PhysicsBodyHandle.h"

namespace ECSEngine
{
class RigidbodyModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(RigidbodyModule, RigidbodyModuleTemplate);

public:
    RigidbodyModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~RigidbodyModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    const Physics::PhysicsBodyConfig& GetBodyConfig() const { return FBodyConfig; }

    SERIALIZE() { PROPERTYFIELD(BodyConfig, Physics::PhysicsBodyConfig()); }

protected:
    void VirtualDrawEditor() override;

private:
    Physics::PhysicsBodyConfig FBodyConfig;
};

class RigidbodyModule : public Module
{
    DECLARE_MODULE(RigidbodyModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    RigidbodyModule();
    ~RigidbodyModule();

    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    void VirtualDeinit() override;

private:
    Physics::PhysicsBodyHandle FHandle;
};

} // namespace ECSEngine
