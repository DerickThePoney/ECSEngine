#pragma once
#include "BuildingCostManager.h"
#include "Common/Singleton.h"
#include "GameplayConstants.h"
#include "PeonSpawningRulesManager.h"

namespace ECSEngine
{
class GameplayRulesManager : public Singleton<GameplayRulesManager>
{
public:
    SERIALIZE()
    {
        NAMEDPROPERTYFIELD("GameplayConstants", FConstants, GameplayConstantsLoader());
        PROPERTYFIELD(PeonSpawningRulesManager, PeonSpawningRulesManager());
        PROPERTYFIELD(BuildingCostManager, BuildingCostManager());
    }

    PeonSpawningRulesManager FPeonSpawningRulesManager;
    BuildingCostManager FBuildingCostManager;

    void DrawConstantsEditor() { FConstants.DrawEditor(); }

private:
    GameplayConstantsLoader FConstants;
};
} // namespace ECSEngine