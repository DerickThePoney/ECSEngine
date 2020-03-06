#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ApplicationUpdater.h"
#include "ECSCore/ECSLoader.h"
#include "RenderingCore/RenderingLoader.h"

int main(int argc, char** argv)
{
    ECSEngine::BaseApplicationLayer app;

    {
        try
        {
            std::ifstream ifstr("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\Configuration\\BaseApplication.json");
            cereal::JSONInputArchive ar(ifstr);
            ar(app);
        }
        catch (std::exception e)
        {
            app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>("D:\\Programmation\\GameEngine\\ECSEngine\\Assets");
            app.AddNewLoader<ECSEngine::ECSLoader>("\\Configuration\\EntityTemplates.json");
            app.AddNewLoader<ECSEngine::RenderingLoader>("Base Application");
            app.SetGameplayUpdater_StealOwnership(new ECSEngine::ApplicationUpdaterWrapper());

            {
                std::ofstream ofstr("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\Configuration\\BaseApplication.json", std::ofstream::out);
                AssertRelease(ofstr.good());
                cereal::JSONOutputArchive outputArchive(ofstr);
                outputArchive(app);
            }

            return -1;
        }
    }

    app.Initialise();

    app.RunMainLoop();

    app.Shutdown();

    return 0;
}