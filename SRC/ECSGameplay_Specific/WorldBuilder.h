#pragma once
#include "WorldGenerationParameters.h"

namespace ECSEngine
{
class WorldBuilder
{
public:
    WorldBuilder(const WorldGenerationParametersDescriptor& parWorldGenerationParameters);

    void CreateWorld() const;

private:
    const WorldGenerationParametersDescriptor& FGenerationParameters;
};

} // namespace ECSEngine
