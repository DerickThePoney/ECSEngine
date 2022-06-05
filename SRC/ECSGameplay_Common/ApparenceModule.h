#pragma once
#include "Common/RenderingHandles.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class ApparenceModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ApparenceModule, ApparenceModuleTemplate);

public:
    ApparenceModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual ~ApparenceModuleTemplate() { }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    const std::string& GFXRepresentationDescriptorName() const { return FGFXRepresentationDescriptorName; }
    bool IsSelectable() const { return FIsSelectable; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(GFXRepresentationDescriptorName, "");
        PROPERTYFIELD(IsSelectable, true);
    }

protected:
    virtual void VirtualDrawEditor() override;

private:
    std::string FGFXRepresentationDescriptorName;

    bool FIsSelectable = true;
};

namespace Rendering
{
class GFXRepresentationProxy;
}

class ApparenceModule final : public Module
{
    DECLARE_MODULE(ApparenceModule);

public:
    ApparenceModule();
    ~ApparenceModule() { }

    Rendering::GFXRepresentationProxy* Proxy() const { return FProxy; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;
    void VirtualDeinit() override;

private:
    Rendering::GFXRepresentationProxy* FProxy = nullptr;
};
} // namespace ECSEngine
