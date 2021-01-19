#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class WorkSystem : public ModuleSystem
{
public:
    WorkSystem();

protected:
    virtual void VirtualUpdate() override;

private:
    std::map<EntityId, u32> FFreeWorkJobs;
    std::list<EntityId> FJobLessPeons;
};
} // namespace ECSEngine
