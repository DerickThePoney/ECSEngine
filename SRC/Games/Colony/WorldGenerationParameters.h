#pragma once
#include "Common/BoundingBox.h"

namespace ECSEngine
{
struct WorldGenerationParametersDescriptor
{
    AABB2f FWorldBoundingBox;

    std::string FColonyTemplateName = "Colony template name";

    std::string FFirePlaceTemplateName = "Starting fire place template name";

    std::string FFoodTemplateName = "Food entity template";
    float FMinFoodRadius = 0.f;
    float FMaxFoodRadius = 0.f;
    u32 FNbFoodEntities = 0;

    std::string FPeonTemplateName = "Peon template name";
    u32 FStartingPeonsNumber = 2;
    float FSpawnRadius = 10.0f;

    SERIALIZE()
    {
        PROPERTYFIELD(WorldBoundingBox, AABB2f());
        PROPERTYFIELD(ColonyTemplateName, "Colony template name");
        PROPERTYFIELD(FirePlaceTemplateName, "Starting fire place template name");
        PROPERTYFIELD(FoodTemplateName, "Food entity template");
        PROPERTYFIELD(MinFoodRadius, 0.f);
        PROPERTYFIELD(MaxFoodRadius, 0.f);
        PROPERTYFIELD(NbFoodEntities, 0);

        PROPERTYFIELD(PeonTemplateName, "Peon template name");
        PROPERTYFIELD(StartingPeonsNumber, 2);
        PROPERTYFIELD(SpawnRadius, 10.0f);
    }
};

} // namespace ECSEngine
