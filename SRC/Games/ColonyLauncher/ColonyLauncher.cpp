#include "stdafx.h"

#include "Application/BaseApplication.h"
#include "Application/CommonLoaders.h"
#include "ECSGameplay_Common/ECSLoader.h"
#include "Games/Colony/GameplaySpecificLoader.h"
#include "Launcher/ApplicationUpdater.h"
#include "Launcher/GameIdentity.h"
#include "Physics/PhysicsLoader.h"
#include "RenderingCore/RenderingLoader.h"
#include "SoundCore/SoundLoader.h"

namespace ECSEngine
{
const char* GetGameAssetsDirectory()
{
    return "..\\Assets\\Colony";
}

void InstallDefaultApplication(BaseApplicationLayer& app)
{
    app.AddNewLoader<LoaderInitialiseCommonResources>();
    app.AddNewLoader<ECSLoader>("\\Configuration\\EntityTemplates.json");
    app.AddNewLoader<SoundLoader>();
    app.AddNewLoader<PhysicsLoader>();
    app.AddNewLoader<RenderingLoader>("BuildingGame");
    app.AddNewLoader<ECSGameplaySpecificLoader>("\\Configuration\\GameplayRules.json");
    app.SetGameplayUpdater_StealOwnership(new ApplicationUpdaterWrapper());
}
} // namespace ECSEngine
