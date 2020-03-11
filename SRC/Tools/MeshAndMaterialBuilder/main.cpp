#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "MeshMaterialApplicationUpdater.h"

int main(int argc, char** argv)
{
    ECSEngine::BaseApplicationLayer app;

    try
    {
        app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>("D:\\Programmation\\GameEngine\\ECSEngine\\Assets");
        app.SetGameplayUpdater_StealOwnership(new ECSEngine::MeshMaterialApplicationUpdaterWrapper());

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