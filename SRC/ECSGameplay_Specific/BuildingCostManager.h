#pragma once
#include "GameResources.h"

namespace ECSEngine
{
using BuildingResourceCost = std::pair<GameResource::Type, u32>;

namespace BuildingCategory
{
enum Type
{
    ENERGY,
    PRODUCTION,
    LENGTH
};

const char* AsString(Type parValue);
} // namespace BuildingCategory

class EntityTemplate;
class BuildingCostDescriptor
{
public:
    const std::string& BuildingTemplateName() const { return FBuildingTemplateName; }
    const EntityTemplate* BuildingTemplate() const;
    const std::vector<BuildingResourceCost>& BuildingCosts() const { return FCosts; }
    const BuildingCategory::Type BuildingType() const { return FBuildingType; }

    void DrawEditorHeader();
    void DrawEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(BuildingTemplateName, "DEFAULT TEMPLATE");
        PROPERTYFIELD(Costs, std::vector<BuildingResourceCost>());
        PROPERTYFIELD(BuildingType, BuildingCategory::ENERGY);
    }

private:
    std::string FBuildingTemplateName = "DEFAULT TEMPLATE";
    std::vector<BuildingResourceCost> FCosts;
    BuildingCategory::Type FBuildingType = BuildingCategory::ENERGY;
};

class BuildingCostManager
{
public:
    SERIALIZE() { PROPERTYFIELD(BuildingsCostRules, std::vector<BuildingCostDescriptor>()); }

    const std::vector<BuildingCostDescriptor>& GetBuildingCostDescriptors() const { return FBuildingsCostRules; }
    std::vector<BuildingResourceCost> GetCostsForBuilding(const std::string& parBuildingName);
    std::vector<BuildingResourceCost> GetCostsForBuilding(const u32 parTemplateIndex);

    void DrawEditor();

private:
    std::vector<BuildingCostDescriptor> FBuildingsCostRules;
};
} // namespace ECSEngine
