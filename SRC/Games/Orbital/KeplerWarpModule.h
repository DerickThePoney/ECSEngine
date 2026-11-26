
#pragma once
#include "Common/MemoryView.h"
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "KeplerPropagator.h"

namespace ECSEngine
{
class KeplerWarpModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(KeplerWarpModule, KeplerWarpModuleTemplate);

public:
    KeplerWarpModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~KeplerWarpModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { }

protected:
    void VirtualDrawEditor() override;
};

class KeplerWarpModule : public Module
{
    DECLARE_MODULE(KeplerWarpModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    KeplerWarpModule();
    ~KeplerWarpModule();

    KeplerPropagatorState& KeplerState() { return FState; }

    const dvec3& Position() const { return FPosition; }
    const dvec3& Velocity() const { return FVelocity; }
    const EntityId& CapturedParent() const { return FCapturedParent; }

    const MemoryView<const vec3> PartsOffsetConst() const { return MemoryView(FPartsOffset.data(), FPartsOffset.size()); }
    MemoryView<vec3> PartsOffset() { return MemoryView(FPartsOffset.data(), FPartsOffset.size()); }
    void SetPartsSize(const u32 size)
    {
        FPartsOffset.clear();
        FPartsOffset.resize(size);
    }

    void SetPosition(const dvec3& Value) { FPosition = Value; }
    void SetVelocity(const dvec3& Value) { FVelocity = Value; }
    void SetCapturedParent(const EntityId& Value) { FCapturedParent = Value; }

private:
    KeplerPropagatorState FState;

    dvec3 FPosition;
    dvec3 FVelocity;

    std::vector<vec3> FPartsOffset;
    EntityId FCapturedParent;
};

} // namespace ECSEngine
