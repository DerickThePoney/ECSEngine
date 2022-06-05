#pragma once
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class EntityId;
class HousingSystem : public ModuleSystem
{
public:
    HousingSystem();

protected:
    void VirtualUpdate() override;

private:
    std::map<EntityId, u32> FFreeHousingSpots;
    std::list<EntityId> FHomelessPeons;
};
} // namespace ECSEngine
