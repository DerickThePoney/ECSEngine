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

    const std::string& PeonTemplateName() const { return FPeonTemplateName; }
    const EntityTemplate* PeonTemplate() const;
    u32 StartingPeonNumber() const { return FStartingPeonsNumber; }
    float SpawnRadius() const { return FSpawnRadius; }

    SERIALIZE()
    {
        PROPERTYFIELD(PeonTemplateName, "Peon template name");
        PROPERTYFIELD(StartingPeonsNumber, 2);
        PROPERTYFIELD(SpawnRadius, 10.0f);
    }

protected:
    void VirtualDrawEditor() override;

private:
    void InitialiseTemplate();

private:
    std::string FPeonTemplateName = "Peon template name";
    const EntityTemplate* FPeonTemplate = nullptr;

    u32 FStartingPeonsNumber = 2;
    float FSpawnRadius = 10.0f;
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