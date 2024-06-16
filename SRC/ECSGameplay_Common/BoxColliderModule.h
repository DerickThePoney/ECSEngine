
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class BoxColliderModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(BoxColliderModule, BoxColliderModuleTemplate);

public:
    BoxColliderModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~BoxColliderModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(Extents, vec3(1.f)); }

    vec3 Extents() const { return FExtents; }

protected:
    void VirtualDrawEditor() override;

private:
    vec3 FExtents = vec3(1.f);
};

class BoxColliderModule : public Module
{
    DECLARE_MODULE(BoxColliderModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    BoxColliderModule();
    ~BoxColliderModule();

protected:
    virtual void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    virtual void VirtualOnLoaded() override;
    virtual void VirtualPostInit() override;

private:
    vec3 FExtents = vec3(1.f);
};

} // namespace ECSEngine
