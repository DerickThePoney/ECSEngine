#pragma once
#include "GameResources.h"

namespace ECSEngine
{
using BuildingResourceCost = std::pair<GameResource::Type, u32>;

class EntityTemplate;
class BuildingCostDescriptor
{
public:
    const std::string& BuildingTemplateName() const { return FBuildingTemplateName; }
    const EntityTemplate* BuildingTemplate() const;
    const std::vector<BuildingResourceCost>& BuildingCosts() const { return FCosts; }

    void DrawEditorHeader();
    void DrawEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(BuildingTemplateName, "DEFAULT TEMPLATE");
        PROPERTYFIELD(Costs, std::vector<BuildingResourceCost>());
    }

private:
    std::string FBuildingTemplateName = "DEFAULT TEMPLATE";
    std::vector<BuildingResourceCost> FCosts;
};

class BuildingCostManager
{
public:
    SERIALIZE() { PROPERTYFIELD(BuildingsCostRules, std::vector<BuildingCostDescriptor>()); }

    const std::vector<BuildingCostDescriptor>& GetCostSpawnRules() const { return FBuildingsCostRules; }
    std::vector<BuildingResourceCost> GetCostsForBuilding(const std::string& parBuildingName);
    std::vector<BuildingResourceCost> GetCostsForBuilding(const u32 parTemplateIndex);

    void DrawEditor();

private:
    std::vector<BuildingCostDescriptor> FBuildingsCostRules;
};
} // namespace ECSEngine