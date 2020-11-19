#include "stdafx.h"

#include "PeonSpawningRulesManager.h"

#include "ECSCore/EntityTemplateManager.h"

namespace ECSEngine
{

const EntityTemplate* PeonSpawningCostRule::PeonTemplate() const
{
    return EntityTemplateManager::Instance().GetEntityTemplate(FPeonTemplateName);
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningCostRule::CostForNextSpawn(const u32 parCurrentPeonsQuantity) const
{
    std::vector<std::pair<GameResource::Type, u32>> res;

    foreachitemconst(costComputer, FPeonCostComputers) { res.push_back({ costComputer.FResourceToPay, costComputer.GetCostForNextSpawn(parCurrentPeonsQuantity) }); }

    return std::move(res);
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningRulesManager::GetCostForNextSpawn(const std::string& parPeonName, const u32 parCurrentPeonsQuantity)
{
    forrange(i, 0, FPeonSpawningRules.size())
    {
        if (FPeonSpawningRules[i].PeonTemplateName() == parPeonName)
        {
            return GetCostForNextSpawn((u32)i, parCurrentPeonsQuantity);
        }
    }
    AssertNotReached();
    return std::move(std::vector<std::pair<GameResource::Type, u32>>());
}

std::vector<std::pair<GameResource::Type, u32>>&& PeonSpawningRulesManager::GetCostForNextSpawn(const u32 parTemplateIndex, const u32 parCurrentPeonsQuantity)
{
    AssertRelease(parTemplateIndex < (u32)FPeonSpawningRules.size());
    return FPeonSpawningRules[parTemplateIndex].CostForNextSpawn(parCurrentPeonsQuantity);
}

} // namespace ECSEngine