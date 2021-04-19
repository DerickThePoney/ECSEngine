#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ApplicationUpdater.h"
#include "Common/MainOptions.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSGameplay_Common/ECSLoader.h"
#include "ECSGameplay_Specific/GameplaySpecificLoader.h"
#include "RenderingCore/RenderingLoader.h"

int main(int argc, char** argv)
{
    ECSEngine::ReadMainCommandLine(argc, argv);

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
            app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>();
            app.AddNewLoader<ECSEngine::ECSLoader>("\\Configuration\\EntityTemplates.json");
            app.AddNewLoader<ECSEngine::RenderingLoader>("Base Application");
            app.AddNewLoader<ECSEngine::ECSGameplaySpecificLoader>("\\Configuration\\GameplayRules.json");
            app.SetGameplayUpdater_StealOwnership(new ECSEngine::ApplicationUpdaterWrapper());

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

    return 0;
}
