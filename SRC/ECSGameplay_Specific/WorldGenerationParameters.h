#pragma once

namespace ECSEngine
{
struct WorldGenerationParameters
{
    std::string FFirePlaceTemplateName = "Starting fire place template name";

    SERIALIZE() { PROPERTYFIELD(FirePlaceTemplateName, "Starting fire place template name"); }
};
} // namespace ECSEngine
