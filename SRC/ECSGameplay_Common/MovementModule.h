#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"

namespace ECSEngine
{
class MovementModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(MovementModule, MovementModuleTemplate);

public:
    MovementModuleTemplate()
        : ModuleTemplate()
    {
    }

    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    template<class Archive>
    void serialize(Archive& ar)
    {
        PROPERTYFIELD(MaxSpeed, 0.f);
        PROPERTYFIELD(MaxRotationSpeed, 0.f);
    }

    float MaxSpeed() const { return FMaxSpeed; }
    float MaxRotationSpeed() const { return FMaxRotationSpeed; }

protected:
    virtual void VirtualDrawEditor() override;

private:
    float FMaxSpeed = 0.f;
    float FMaxRotationSpeed = 0.f;
};

class MovementModule final : public Module
{
    DECLARE_MODULE(MovementModule);

public:
    MovementModule();
    ~MovementModule() { }

    void SetNewPath(const std::vector<glm::vec2>& parNewPath);
    void SetCurrentFollowedWaypoint(const u32 parNewValue) { FCurrentFollowedWaypoint = parNewValue; }
    void SetRequestIsPending(const bool parValue) { FRequestIsPending = parValue; }
    void SetCurrentSpeed(const glm::vec3 parNewSpeed) { FCurrentSpeed = parNewSpeed; }

    const std::vector<glm::vec2>& Path() const { return FPath; }
    u32 CurrentFollowedWayPoint() const { return FCurrentFollowedWaypoint; }
    bool RequestIsPending() const { return FRequestIsPending; }
    glm::vec3 CurrentSpeed() const { return FCurrentSpeed; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    glm::vec3 FCurrentSpeed = glm::vec3(0.f);

    std::vector<glm::vec2> FPath;
    u32 FCurrentFollowedWaypoint = -1;
    bool FRequestIsPending = false;
};
} // namespace ECSEngine