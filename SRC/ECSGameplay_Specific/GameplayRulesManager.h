#pragma once
#include "BuildingCostManager.h"
#include "Common/Singleton.h"
#include "GameplayConstants.h"
#include "ProductionRecipesManager.h"

namespace ECSEngine
{
class GameplayRulesManager : public Singleton<GameplayRulesManager>
{
public:
    SERIALIZE()
    {
        NAMEDPROPERTYFIELD("GameplayConstants", FConstants, GameplayConstantsLoader());
        PROPERTYFIELD(BuildingCostManager, BuildingCostManager());
        PROPERTYFIELD(ProductionRecipesManager, ProductionRecipesManager());
    }

    BuildingCostManager FBuildingCostManager;
    ProductionRecipesManager FProductionRecipesManager;

    void DrawConstantsEditor() { FConstants.DrawEditor(); }

private:
    GameplayConstantsLoader FConstants;
};
} // namespace ECSEngine
