#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ApplicationUpdater.h"
#include "Common/CallStack.h"
#include "Common/MainOptions.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSGameplay_Common/ECSLoader.h"
#include "GameIdentity.h"
#include "Physics/PhysicsLoader.h"
#include "RenderingCore/RenderingLoader.h"
#include "SoundCore/SoundLoader.h"

#include <fstream>

int main(int argc, char** argv)
{
    ECSEngine::CallStack::InitializeSymbols();
    ECSEngine::ReadMainCommandLine(argc, argv);
#ifndef COMPILE_FINAL
    if (!ECSEngine::Options.AssetsDirectoryOverridden)
        ECSEngine::Configuration::AssetsDirectory = ECSEngine::GetGameAssetsDirectory();
#endif

    ECSEngine::Profiling::StartProfiler();
    if (!ECSEngine::InitialiseGlobalCache())
    {
        ECSEngine::DestroyGlobalCache();
        ECSEngine::Profiling::EndProfiler();
        return -1;
    }

    ECSEngine::BaseApplicationLayer app;

    {
        try
        {
            ECSEngine::Resource baseApplication("Configuration\\BaseApplication.json");
            std::shared_ptr<ECSEngine::ResourceHandle> handle = ECSEngine::GlobalResourceCache::Instance().FCache->GetResourceHandle(&baseApplication);
            ECSEngine::ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream sstr(&buff, std::istream::in);
            cereal::JSONInputArchive ar(sstr);
            ar(app);
        }
        catch (std::exception e)
        {
#ifndef COMPILE_FINAL
            AssertNotReached();
            ECSEngine::InstallDefaultApplication(app);

            {
                std::ofstream ofstr("..\\Assets\\Configuration\\BaseApplication.json", std::ofstream::out);
                AssertRelease(ofstr.good());
                cereal::JSONOutputArchive outputArchive(ofstr);
                outputArchive(app);
            }
#endif

            return -1;
        }
    }

    app.Initialise();

    app.RunMainLoop();

    app.Shutdown();

    ECSEngine::DestroyGlobalCache();
    ECSEngine::Profiling::EndProfiler();
    ECSEngine::CallStack::CleanupSymbols();

    return 0;
}
