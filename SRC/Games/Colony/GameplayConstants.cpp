#include "stdafx.h"

#include "GameplayConstants.h"

#include "Application/PropertyDrawer.h"

namespace ECSEngine
{
namespace GameplayConstants
{
namespace PeonFeeding
{
u32 PeonEatQuantity = 1;
}
namespace Colony
{
float ColonyInitialRange = 20.f;
float ColonyRangeFeedbackThickness = 1.f;
vec4 ColonyRangeFeedbackColor = vec4(0.f);
} // namespace Colony
namespace CircularBuildingGrid
{
float WantedArcLength = 2.0f;
float InterChunkLength = 0.5f;
float GridStartRadius = 3.0f;
float GridChunkWidth = 1.0f;

float NavigationNodesDistance = 0.5f;

float GridChunkFeedbackThickness = 0.1f;
u32 StartingGridChunkNumber = 2;
vec4 GridFeedbackColor = vec4(0.f);
} // namespace CircularBuildingGrid

namespace Storage
{
vec4 HighlightedColor = vec4(1.f);
vec4 SelectedColor = vec4(1.f);
float CircleThickness = 0.1f;
} // namespace Storage

namespace Energy
{
EnergyEfficiencyDescriptor EnergyEfficiency;
}
} // namespace GameplayConstants

void GameplayConstantsLoader::PostSerialize()
{
    // Peon feeding
    GameplayConstants::PeonFeeding::PeonEatQuantity = FPeonEatQuantity;

    // Colony
    GameplayConstants::Colony::ColonyInitialRange = FColonyInitialRange;
    GameplayConstants::Colony::ColonyRangeFeedbackThickness = FColonyRangeFeedbackThickness;
    GameplayConstants::Colony::ColonyRangeFeedbackColor = FColonyRangeFeedbackColor;

    // CircularBuildingGrid
    GameplayConstants::CircularBuildingGrid::WantedArcLength = FWantedArcLength;
    GameplayConstants::CircularBuildingGrid::InterChunkLength = FInterChunkLength;
    GameplayConstants::CircularBuildingGrid::GridStartRadius = FGridStartRadius;
    GameplayConstants::CircularBuildingGrid::GridChunkWidth = FGridChunkWidth;
    GameplayConstants::CircularBuildingGrid::NavigationNodesDistance = FNavigationNodesDistance;
    GameplayConstants::CircularBuildingGrid::GridChunkFeedbackThickness = FGridChunkFeedbackThickness;
    GameplayConstants::CircularBuildingGrid::StartingGridChunkNumber = FStartingGridChunkNumber;
    GameplayConstants::CircularBuildingGrid::GridFeedbackColor = FGridFeedbackColor;

    // Storage
    GameplayConstants::Storage::HighlightedColor = FHighlightedColor;
    GameplayConstants::Storage::SelectedColor = FSelectedColor;
    GameplayConstants::Storage::CircleThickness = FCircleThickness;

    // Energy
    GameplayConstants::Energy::EnergyEfficiency = FEnergyEfficiency;
}

void GameplayConstantsLoader::DrawEditor()
{
    if (ImGui::CollapsingHeader("PeonFeeding"))
    {
        EDITOR_PROPERTY_SIMPLE("Peon eat quantity", FPeonEatQuantity);
    }

    if (ImGui::CollapsingHeader("Colony"))
    {
        EDITOR_PROPERTY_SIMPLE("Colony initial range", FColonyInitialRange);
        EDITOR_PROPERTY_SIMPLE("Colony range feedback thickness", FColonyRangeFeedbackThickness);
        EDITOR_PROPERTY_COLOR("Colony range feedback color", FColonyRangeFeedbackColor);
    }

    if (ImGui::CollapsingHeader("Circular building grid parameters"))
    {
        EDITOR_PROPERTY_SIMPLE("Wanted arc length", FWantedArcLength);
        EDITOR_PROPERTY_SIMPLE("Inter chunk length", FInterChunkLength);
        EDITOR_PROPERTY_SIMPLE("Grid start radius", FGridStartRadius);
        EDITOR_PROPERTY_SIMPLE("Grid chunk width", FGridChunkWidth);
        EDITOR_PROPERTY_SIMPLE("Grid chunks unlocked at start", FStartingGridChunkNumber);
        ImGui::Separator();
        EDITOR_PROPERTY_WITH_LIMITS("Navigation node distance", FNavigationNodesDistance, 0.f, FWantedArcLength);
        ImGui::Separator();
        EDITOR_PROPERTY_SIMPLE("Grid chunk feedback thickness", FGridChunkFeedbackThickness);
        EDITOR_PROPERTY_COLOR("Grid feedback color", FGridFeedbackColor);
    }

    if (ImGui::CollapsingHeader("Storage building parameters"))
    {
        EDITOR_PROPERTY_COLOR("Storage range feedback highlighted color", FHighlightedColor);
        EDITOR_PROPERTY_COLOR("Storage range feedback selected color", FSelectedColor);
        EDITOR_PROPERTY_SIMPLE("Storage range thickness", FCircleThickness);
    }

    if (ImGui::CollapsingHeader("Energy parameters"))
    {
        FEnergyEfficiency.DrawEditor();
    }

    PostSerialize();
}

} // namespace ECSEngine
