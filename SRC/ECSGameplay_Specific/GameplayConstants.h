#pragma once

namespace ECSEngine
{
namespace GameplayConstants
{
namespace PeonFeeding
{
extern u32 PeonEatQuantity;
}
namespace Colony
{
extern float ColonyInitialRange;
extern float ColonyRangeFeedbackThickness;
extern glm::vec4 ColonyRangeFeedbackColor;
} // namespace Colony
namespace CircularBuildingGrid
{
extern float WantedArcLength;
extern float InterChunkLength;
extern float GridStartRadius;
extern float GridChunkWidth;
extern float NavigationNodesDistance;
extern float GridChunkFeedbackThickness;
extern u32 StartingGridChunkNumber;
extern glm::vec4 GridFeedbackColor;
} // namespace CircularBuildingGrid
} // namespace GameplayConstants

// work it out as actual extern X Y;
class GameplayConstantsLoader
{
public:
    SERIALIZE()
    {
        PROPERTYFIELD(PeonEatQuantity, 1);

        PROPERTYFIELD(ColonyInitialRange, 20.f);
        PROPERTYFIELD(ColonyRangeFeedbackThickness, 1.f);
        PROPERTYFIELD(ColonyRangeFeedbackColor, glm::vec4(0.f));

        // CircularBuildingGrid
        PROPERTYFIELD(WantedArcLength, 2.0f);
        PROPERTYFIELD(InterChunkLength, 0.5f);
        PROPERTYFIELD(GridStartRadius, 3.0f);
        PROPERTYFIELD(GridChunkWidth, 1.0f);
        PROPERTYFIELD(NavigationNodesDistance, 0.5f);
        PROPERTYFIELD(GridChunkFeedbackThickness, 0.1f);
        PROPERTYFIELD(StartingGridChunkNumber, 2);
        PROPERTYFIELD(GridFeedbackColor, glm::vec4(0.f));

        PostSerialize();
    }

    void PostSerialize();

    void DrawEditor();

private:
    // Peon Feeding
    u32 FPeonEatQuantity = 1;

    // Colony
    float FColonyInitialRange = 20.f;
    float FColonyRangeFeedbackThickness = 1.f;
    glm::vec4 FColonyRangeFeedbackColor = glm::vec4(0.f);

    // CircularBuildingGrid
    float FWantedArcLength = 2.0f;
    float FInterChunkLength = 0.5f;
    float FGridStartRadius = 3.0f;
    float FGridChunkWidth = 1.0f;
    float FNavigationNodesDistance = 0.5f;
    float FGridChunkFeedbackThickness = 0.1f;
    u32 FStartingGridChunkNumber = 2;
    glm::vec4 FGridFeedbackColor = glm::vec4(0.f);
};
} // namespace ECSEngine