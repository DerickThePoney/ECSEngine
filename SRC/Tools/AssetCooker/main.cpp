#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "AssetCookerUpdater.h"

int main(int argc, char** argv)
{
    ECSEngine::BaseApplicationLayer app;

    try
    {
        app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>("..\\Assets");
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

    return 0;
}