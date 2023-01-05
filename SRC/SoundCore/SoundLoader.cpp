#include "stdafx.h"

#include "SoundLoader.h"

#include "SoundEngineHelpers.h"
#include "SoundResourceFileSystem.h"

namespace ECSEngine
{

bool SoundLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    SoundResources::InitializeCache();

    SoundEngine::Initialize();
    return true;
}

void SoundLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();
    SoundEngine::Shutdown();
    SoundResources::DestroyCache();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::SoundLoader);