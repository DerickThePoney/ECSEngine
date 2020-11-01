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
    }

protected:
    virtual void VirtualDrawEditor() override;
};

class MovementModule final : public Module
{
    DECLARE_MODULE(MovementModule);

public:
    MovementModule();
    ~MovementModule() { }

    void SetNewPath(const std::vector<glm::vec2>& parNewPath);
    void SetRequestIsPending(const bool parValue) { FRequestIsPending = parValue; }

    const std::vector<glm::vec2>& Path() const { return FPath; }
    u32 CurrentFollowedWayPoint() const { return FCurrentFollowedWaypoint; }
    bool RequestIsPending() const { return FRequestIsPending; }

protected:
    void VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) override;

private:
    std::vector<glm::vec2> FPath;
    u32 FCurrentFollowedWaypoint = -1;
    bool FRequestIsPending = false;
};
} // namespace ECSEngine