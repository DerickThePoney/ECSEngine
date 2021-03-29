#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class ColonyFeedbackSystem : public ModuleSystem
{
public:
    ColonyFeedbackSystem();

protected:
    void VirtualUpdate();
};
} // namespace ECSEngine
