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
class ColonySelectionPanelController : public UIControllerWithModuleAccessors<MC<ColonyModule, Worlds::COLONY>,
                                             MC<ResourceStorageModule, Worlds::COLONY>,
                                             MC<PeonSpawnModule, Worlds::COLONY>,
                                             MC<ColonyPeonsManagementModule, Worlds::COLONY>,
                                             MC<PeonFeedingTimeModule, Worlds::COLONY>,
                                             MC<HousingPlaceModule, Worlds::BUILDINGS>>
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
