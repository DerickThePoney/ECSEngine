#pragma once
#include "ColonyModule.h"
#include "ColonyPeonsManagementModule.h"
#include "ECSCore/UIController.h"
#include "ECSCore/WorldIds.h"
#include "HousingPlaceModule.h"
#include "PeonFeedingTimeModule.h"
#include "PeonSpawnModule.h"
#include "ResourceStorageModule.h"

namespace ECSEngine
{
namespace UI
{
class ColonySelectionPanelController : public UIControllerWithModuleAccessors<MC<ColonyModule, EEntityWorlds::COLONY>,
                                             MC<ResourceStorageModule, EEntityWorlds::COLONY>,
                                             MC<PeonSpawnModule, EEntityWorlds::COLONY>,
                                             MC<ColonyPeonsManagementModule, EEntityWorlds::COLONY>,
                                             MC<PeonFeedingTimeModule, EEntityWorlds::COLONY>,
                                             MC<HousingPlaceModule, EEntityWorlds::BUILDINGS>>
{
public:
    ColonySelectionPanelController();
    ~ColonySelectionPanelController();

protected:
    void VirtualUpdate() override;

private:
};
} // namespace UI
} // namespace ECSEngine
