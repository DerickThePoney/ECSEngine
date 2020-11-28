#pragma once
#include "Common/MemoryView.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "GameResources.h"

namespace ECSEngine
{
class EntityId;

namespace ModuleParameters
{
class ParameterContainer;
}

class ResourceStorageModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ResourceStorageModule, ResourceStorageModuleTemplate);
    using StartingResources = std::pair<GameResource::Type, u32>;

public:
    ResourceStorageModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(MaxResourceQuantity, 10);
        PROPERTYFIELD(StartingResources, std::vector<StartingResources>());
    }

    u32 MaxResourceQuantity() const { return FMaxResourceQuantity; }
    MemoryView<const StartingResources> GetStartingResources() const { return MemoryView<const StartingResources>(FStartingResources.data(), (u32)FStartingResources.size()); }

protected:
    virtual void VirtualDrawEditor() override;

#ifdef PERFORM_SECURITY_CHECKS
    virtual void VirtualVerifyTemplate() const override;
#endif

private:
    u32 FMaxResourceQuantity;
    std::vector<StartingResources> FStartingResources;
};

class ResourcesStatistics
{
    constexpr static float MaxTimeForMovingAverage = 5.f;

public:
    void AddResourceChange(const GameResource::Type parResource, const i32 parQuantity);
    float GetAverageResourcePerUnitOfTime(const GameResource::Type parResource) const;

    void UpdateStatistics(const float parNow);

private:
    struct ResourceData
    {
        std::list<std::pair<float, i32>> Changes;
        float AverageResourcePerUnitOfTime = 0.f;
    };

    std::map<GameResource::Type, ResourceData> FStatistics;
};

class ResourceStorageModule final : public Module
{
    DECLARE_MODULE(ResourceStorageModule);

public:
    ResourceStorageModule();
    ~ResourceStorageModule() { }

    u32 GetResourceQuantity(const GameResource::Type parResource) const;
    u32 GetRemainingStorageSpace() const;

    u32 AddResource(const GameResource::Type parResource, const u32 parQuantity);
    u32 RemoveResource(const GameResource::Type parResource, const u32 parQuantity);

    GameResource::Type GetMainResource() const;

    ResourcesStatistics& Statistics() { return FResourceStatisticsManager; }
    const ResourcesStatistics& Statistics() const { return FResourceStatisticsManager; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    using CarriedResource = std::pair<GameResource::Type, u32>;
    std::vector<CarriedResource> FCarriedResources;

    ResourcesStatistics FResourceStatisticsManager;
};
} // namespace ECSEngine
