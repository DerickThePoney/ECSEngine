#include "stdafx.h"

#include "ColonyFeedbackSystem.h"

#include "ColonyTraitsModule.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"
#include "CircularBuildingGrid.h"

namespace ECSEngine
{

ColonyFeedbackSystem::ColonyFeedbackSystem()
{
    RegisterDepency<PositionModule>(Worlds::COLONY);
    RegisterDepency<ColonyTraitsModule>(Worlds::COLONY);
}

void ColonyFeedbackSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    ModuleAccessor<ColonyTraitsModule> colonyTraitsAccessor(Worlds::COLONY);
    ModuleAccessor<PositionModule> colonyPositionAccessor(Worlds::COLONY);

    foreachitemconst(traits, colonyTraitsAccessor)
    {
        const PositionModule* colonyPositionModule = colonyPositionAccessor[traits.UnitId()];
        AssertRelease(colonyPositionModule != nullptr);

        GameplayFeedbackDrawer::Instance().AddCircle(
              { traits.InfluenceRange(), GameplayConstants::Colony::ColonyRangeFeedbackThickness, GameplayConstants::Colony::ColonyRangeFeedbackColor },
              glm::translate(colonyPositionModule->GetPosition3D()));
    }

    if (CircularBuildingGrid::HasInstance())
    {
        CircularBuildingGrid::Instance().DrawFeedback();
    }
}

} // namespace ECSEngine
