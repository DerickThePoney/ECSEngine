#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "AssetCookerUpdater.h"
#include "Common/CallStack.h"
#include "Common/MainOptions.h"

namespace ECSEngine
{
MainOptions Options;
}

int main(int argc, char** argv)
{
    ECSEngine::CallStack::InitializeSymbols();
#ifndef COMPILE_FINAL
    ECSEngine::Options.NoDatapack = true;
#endif

    ECSEngine::BaseApplicationLayer app;

    if (!ECSEngine::InitialiseGlobalCache())
    {
        ECSEngine::DestroyGlobalCache();
        return -1;
    }

    try
    {
        app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>();
        app.SetGameplayUpdater_StealOwnership(new ECSEngine::AssetCookerUpdaterWrapper());

        app.Initialise();
        app.RunMainLoop();
        app.Shutdown();
    }
    catch (std::exception& e)
    {
        std::cerr << "Unhandled exception!" << std::endl << e.what() << std::endl;
        return -1;
    }

    // destroy Resources
    ECSEngine::DestroyGlobalCache();

    ECSEngine::CallStack::CleanupSymbols();

    return 0;
}
