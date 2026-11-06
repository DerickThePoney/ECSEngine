
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class NozzleEngineModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(NozzleEngineModule, NozzleEngineModuleTemplate);

public:
    NozzleEngineModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~NozzleEngineModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE()
    {
        PROPERTYFIELD(MaxThrust, 0.f);
        PROPERTYFIELD(ISP, 0.f);
        PROPERTYFIELD(OffOnBoosters, false);
    }

    float MaxThrust() const { return FMaxThrust; }
    float ISP() const { return FISP; }
    bool OffOnBoosters() const { return FOffOnBoosters; }

protected:
    void VirtualDrawEditor() override;

    float FMaxThrust = 0.f;
    float FISP = 0.f;
    bool FOffOnBoosters = false;
};

class NozzleEngineModule : public Module
{
    DECLARE_MODULE(NozzleEngineModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    NozzleEngineModule();
    ~NozzleEngineModule();

    float MaxThrust() const;
    float ISP() const;
    bool OffOnBoosters() const;

    float Throttle() const { return FThrottle; }
    void SetThrottle(float Value);

private:
    float FThrottle = 0.f;
};

} // namespace ECSEngine
