#pragma once
#include "ECSCore/EntityId.h"
#include "PeonSpawningRulesManager.h"

namespace ECSEngine
{
template<typename Action>
class OrderExecutor
{
public:
    OrderExecutor(Action&& parAction)
        : FAction(std::move(parAction))
    {
    }

    void ExecuteOrder() { FAction.Execute(); }

private:
    Action FAction;
};

class SpawnPeonOrder
{
public:
    SpawnPeonOrder() { }
    SpawnPeonOrder(const EntityId& parColonyId, const PeonSpawningCostRule& parPeonCostRule)
        : FColonyId(parColonyId)
        , FPeonCostRule(parPeonCostRule)
    {
    }

    void Execute();

private:
    EntityId FColonyId;
    PeonSpawningCostRule FPeonCostRule;
};
} // namespace ECSEngine
