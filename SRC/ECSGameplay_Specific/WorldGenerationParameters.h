#pragma once

namespace ECSEngine
{
struct WorldGenerationParametersDescriptor
{
    std::string FColonyTemplateName = "Colony template";

    std::string FFirePlaceTemplateName = "Starting fire place template name";

    std::string FFoodTemplateName = "Food entity template";
    float FMinFoodRadius = 0.f;
    float FMaxFoodRadius = 0.f;
    u32 FNbFoodEntities = 0;

    SERIALIZE()
    {
        PROPERTYFIELD(ColonyTemplateName, "Colony template");
        PROPERTYFIELD(FirePlaceTemplateName, "Starting fire place template name");
        PROPERTYFIELD(FoodTemplateName, "Food entity template");
        PROPERTYFIELD(MinFoodRadius, 0.f);
        PROPERTYFIELD(MaxFoodRadius, 0.f);
        PROPERTYFIELD(NbFoodEntities, 0);
    }
};

} // namespace ECSEngine
