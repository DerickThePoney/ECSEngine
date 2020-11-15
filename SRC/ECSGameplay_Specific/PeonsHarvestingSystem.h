#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class PeonsHaverstingSystem : public ModuleSystem
{
public:
    PeonsHaverstingSystem();
    ~PeonsHaverstingSystem();

    void Debug();

protected:
    void VirtualUpdate() override;
};
} // namespace ECSEngine