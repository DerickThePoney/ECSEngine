#pragma once
#include "PeonCostComputer.h"

namespace ECSEngine
{
class EntityTemplate;
class PeonSpawningRulesManager;
class PeonSpawningCostRule
{
    friend PeonSpawningRulesManager;

public:
    const std::string& PeonTemplateName() const { return FPeonTemplateName; }
    const EntityTemplate* PeonTemplate() const;
    std::vector<std::pair<GameResource::Type, u32>> CostForNextSpawn(const u32 parCurrentPeonsQuantity) const;
    u32 CostRulesNumber() const { return (u32)FPeonCostComputers.size(); }

    static void DrawEditingHeader();
    void DrawEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(PeonTemplateName, "DEFAULT TEMPLATE");
        PROPERTYFIELD(PeonCostComputers, std::vector<PeonCostComputer>())
    }

private:
    std::string FPeonTemplateName = "DEFAULT TEMPLATE";
    std::vector<PeonCostComputer> FPeonCostComputers;
};

class PeonSpawningRulesManager
{
public:
    SERIALIZE()
    {
        PROPERTYFIELD(PeonSpawningRules, std::vector<PeonSpawningCostRule>());
        PROPERTYFIELD(AutoSpawn, false);
    }

    const std::vector<PeonSpawningCostRule>& GetCostSpawnRules() const { return FPeonSpawningRules; }
    std::vector<std::pair<GameResource::Type, u32>> GetCostForNextSpawn(const std::string& parPeonName, const u32 parCurrentPeonsQuantity);
    std::vector<std::pair<GameResource::Type, u32>> GetCostForNextSpawn(const u32 parTemplateIndex, const u32 parCurrentPeonsQuantity);

    bool AutoSpawn() const { return FAutoSpawn; }

    void DrawEditor();

private:
    bool FAutoSpawn = false;
    std::vector<PeonSpawningCostRule> FPeonSpawningRules;
};
} // namespace ECSEngine
