#pragma once
#include "Common/Singleton.h"
#include "PeonSpawningRulesManager.h"

namespace ECSEngine
{
class GameplayRulesManager : public Singleton<GameplayRulesManager>
{
public:
    SERIALIZE() { PROPERTYFIELD(PeonSpawningRulesManager, PeonSpawningRulesManager()); }

    PeonSpawningRulesManager FPeonSpawningRulesManager;
};
} // namespace ECSEngine