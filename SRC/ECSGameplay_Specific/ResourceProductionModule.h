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

class ResourceProductionModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(ResourceProductionModule, ResourceProductionModuleTemplate);
    using ProducedResource = std::pair<GameResource::Type, float>;

public:
    ResourceProductionModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(ProducedResources, std::vector<ProducedResource>());
    }

    MemoryView<const ProducedResource> ProducedResources() const { return MemoryView<const ProducedResource>(FProducedResources.data(), (u32)FProducedResources.size()); }

protected:
    virtual void VirtualDrawEditor() override;

private:
    std::vector<ProducedResource> FProducedResources;
};

class ResourceProductionModule final : public Module
{
    DECLARE_MODULE(ResourceProductionModule);

public:
    using ProducedResourceTiming = std::pair<GameResource::Type, float>;

    ResourceProductionModule();
    ~ResourceProductionModule() { }

    MemoryView<ProducedResourceTiming> ProducedResourcesTimings()
    {
        return MemoryView<ProducedResourceTiming>(FRemainingProductionTimes.data(), (u32)FRemainingProductionTimes.size());
    }
    MemoryView<const ProducedResourceTiming> ProducedResourcesTimings() const
    {
        return MemoryView<const ProducedResourceTiming>(FRemainingProductionTimes.data(), (u32)FRemainingProductionTimes.size());
    }

    void ResetTimingForResource(const GameResource::Type parResource);

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<ProducedResourceTiming> FRemainingProductionTimes;
};
} // namespace ECSEngine
