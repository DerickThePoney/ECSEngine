#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ApplicationUpdater.h"
#include "ECSGameplay_Common/ECSLoader.h"
#include "ECSGameplay_Specific/GameplaySpecificLoader.h"
#include "RenderingCore/RenderingLoader.h"

int main(int argc, char** argv)
{
    ECSEngine::BaseApplicationLayer app;

    {
        try
        {
            std::ifstream ifstr("..\\Assets\\Configuration\\BaseApplication.json");
            cereal::JSONInputArchive ar(ifstr);
            ar(app);
        }
        catch (std::exception e)
        {
            app.AddNewLoader<ECSEngine::LoaderInitialiseCommonResources>("..\\Assets");
            app.AddNewLoader<ECSEngine::ECSLoader>("\\Configuration\\EntityTemplates.json");
            app.AddNewLoader<ECSEngine::RenderingLoader>("Base Application");
            app.AddNewLoader<ECSEngine::ECSGameplaySpecificLoader>("\\Configuration\\GameplayRules.json");
            app.SetGameplayUpdater_StealOwnership(new ECSEngine::ApplicationUpdaterWrapper());

            {
                std::ofstream ofstr("Assets\\Configuration\\BaseApplication.json", std::ofstream::out);
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