#include "stdafx.h"

#include "SoundEngineHelpers.h"

#include "Common/MainOptions.h"
#include "ISoundEngine.h"

namespace ECSEngine
{
namespace SoundEngine
{

void Initialize()
{
    ISoundEngine* soundEngine = SoundEngine::Get();
    AssertRelease(soundEngine != nullptr);
    soundEngine->Initialize(Options.NoSound);
}

void Shutdown()
{
    ISoundEngine* soundEngine = SoundEngine::Get();
    AssertRelease(soundEngine != nullptr);
    soundEngine->Shutdown();
}

} // namespace SoundEngine
} // namespace ECSEngine
