#include "stdafx.h"

#include "SoundLoader.h"

#include "SoundEngineHelpers.h"

namespace ECSEngine
{

bool SoundLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();
    SoundEngine::Initialize();
    return true;
}

void SoundLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();
    SoundEngine::Shutdown();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::SoundLoader);