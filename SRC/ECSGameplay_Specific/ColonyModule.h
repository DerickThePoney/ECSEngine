#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}

class EntityId;

class ColonyModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ColonyModule, ColonyModuleTemplate);

public:
    ColonyModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class ColonyModule final : public Module
{
    DECLARE_MODULE(ColonyModule);

public:
    ColonyModule();
    ~ColonyModule() { }

    const std::string& Name() const { return FName; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::string FName = "COLONY NAME DEFAULT";
};
} // namespace ECSEngine