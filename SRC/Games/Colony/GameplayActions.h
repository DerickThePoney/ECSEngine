#pragma once
#include "ECSCore/EntityId.h"

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
} // namespace ECSEngine
