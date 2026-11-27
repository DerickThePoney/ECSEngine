
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class PhysicsFloatingOriginModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(PhysicsFloatingOriginModule, PhysicsFloatingOriginModuleTemplate);

public:
    PhysicsFloatingOriginModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~PhysicsFloatingOriginModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(BubbleRadius, 2000.f); }

    const double BubbleRadius() const { return FBubbleRadius; }

protected:
    void VirtualDrawEditor() override;

private:
    double FBubbleRadius = 2000.f;
};

class PhysicsFloatingOriginModule : public Module
{
    DECLARE_MODULE(PhysicsFloatingOriginModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    PhysicsFloatingOriginModule();
    ~PhysicsFloatingOriginModule();

    const dvec3& Origin() const { return FOrigin; }
    void SetOrigin(const dvec3& Value) { FOrigin = Value; }

    double BubbleRadius() const { return FBubbleRadius; }
    void SetBubbleRadius(double Radius);

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    dvec3 FOrigin;
    double FBubbleRadius = 2000.0;
};

} // namespace ECSEngine
