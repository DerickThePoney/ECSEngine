
#pragma once
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
    const vec3& OffsetBody() const { return FOffsetBody; }
    const vec3& OffsetNozzle() const { return FOffsetNozzle; }
    const EntityId& CapturedParent() const { return FCapturedParent; }

    void SetPosition(const dvec3& Value) { FPosition = Value; }
    void SetVelocity(const dvec3& Value) { FVelocity = Value; }
    void SetOffsetBody(const vec3& Value) { FOffsetBody = Value; }
    void SetOffsetNozzle(const vec3& Value) { FOffsetNozzle = Value; }
    void SetCapturedParent(const EntityId& Value) { FCapturedParent = Value; }

private:
    KeplerPropagatorState FState;

    dvec3 FPosition;
    dvec3 FVelocity;

    vec3 FOffsetBody;
    vec3 FOffsetNozzle;
    EntityId FCapturedParent;
};

} // namespace ECSEngine
