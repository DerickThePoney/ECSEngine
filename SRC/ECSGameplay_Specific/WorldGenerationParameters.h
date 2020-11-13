#pragma once

namespace ECSEngine
{
struct WorldGenerationParametersDescriptor
{
    std::string FColonyTemplateName = "Colony template";
    std::string FFirePlaceTemplateName = "Starting fire place template name";
    std::string FFoodTemplateName = "Food entity template";

    SERIALIZE()
    {
        PROPERTYFIELD(ColonyTemplateName, "Colony template");
        PROPERTYFIELD(FirePlaceTemplateName, "Starting fire place template name");
        PROPERTYFIELD(FoodTemplateName, "Food entity template");
    }
};
} // namespace ECSEngine
