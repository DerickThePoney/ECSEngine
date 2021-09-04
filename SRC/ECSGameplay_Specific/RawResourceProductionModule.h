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

class RawResourceProductionModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(RawResourceProductionModule, RawResourceProductionModuleTemplate);
    using ProducedRawResource = std::pair<GameResource::Type, float>;

public:
    RawResourceProductionModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(ProducedRawResources, std::vector<ProducedRawResource>());
    }

    MemoryView<const ProducedRawResource> ProducedRawResources() const
    {
        return MemoryView<const ProducedRawResource>(FProducedRawResources.data(), (u32)FProducedRawResources.size());
    }

protected:
    virtual void VirtualDrawEditor() override;

private:
    std::vector<ProducedRawResource> FProducedRawResources;
};

class RawResourceProductionModule final : public Module
{
    DECLARE_MODULE(RawResourceProductionModule);

public:
    using ProducedRawResourceTiming = std::pair<GameResource::Type, float>;

    RawResourceProductionModule();
    ~RawResourceProductionModule() { }

    MemoryView<ProducedRawResourceTiming> ProducedResourcesTimings()
    {
        return MemoryView<ProducedRawResourceTiming>(FRemainingProductionTimes.data(), (u32)FRemainingProductionTimes.size());
    }
    MemoryView<const ProducedRawResourceTiming> ProducedResourcesTimings() const
    {
        return MemoryView<const ProducedRawResourceTiming>(FRemainingProductionTimes.data(), (u32)FRemainingProductionTimes.size());
    }

    void ResetTimingForResource(const GameResource::Type parResource);

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<ProducedRawResourceTiming> FRemainingProductionTimes;
};
} // namespace ECSEngine
