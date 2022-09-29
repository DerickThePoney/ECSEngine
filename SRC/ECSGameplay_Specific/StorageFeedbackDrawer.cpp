#include "stdafx.h"

#include "StorageFeedbackDrawer.h"

#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "ECSGameplay_Common/SelectionManager.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"
#include "RenderingCore/FeedbackParameters.h"
#include "StorageSlotModule.h"

namespace ECSEngine
{

void StorageFeedbackDrawer::DrawFeedback()
{
    const std::set<EntityId>& highlightedUnits = SelectionManager::Instance().HighlightedUnits();
    const std::set<EntityId>& selectedUnits = SelectionManager::Instance().SelectedUnits();
    ManualLockModuleAccessor<StorageSlotModule> storageSlotAccessor(EEntityWorlds::BUILDINGS);
    ManualLockModuleAccessor<PositionModule> positionAccessor(EEntityWorlds::BUILDINGS);
    storageSlotAccessor.LockIFN();
    positionAccessor.LockIFN();

    auto DrawCicle = [](vec4& color, float radius, const mat4& position)
    {
        Rendering::CircleFeedbackParameters param;
        param.Color = color;
        param.Range = radius;
        param.Thickness = GameplayConstants::Storage::CircleThickness;

        GameplayFeedbackDrawer::Instance().AddCircle(param, position);
    };

    foreachitemconst(unit, highlightedUnits)
    {
        const StorageSlotModule* storageModule = storageSlotAccessor[unit];
        if (storageModule == nullptr)
            continue;

        const StorageSlotModuleTemplate* storageTemplate = storageModule->Template<StorageSlotModuleTemplate>();
        AssertRelease(storageTemplate != nullptr);

        const PositionModule* positionModule = positionAccessor[unit];
        AssertRelease(positionModule != nullptr);

        const mat4 positionMatrix = Translation(positionModule->GetPosition3D());

        DrawCicle(GameplayConstants::Storage::HighlightedColor, storageTemplate->RadiusOfEffect(), positionMatrix);
    }

    foreachitemconst(unit, selectedUnits)
    {
        const StorageSlotModule* storageModule = storageSlotAccessor[unit];
        if (storageModule == nullptr)
            continue;

        const StorageSlotModuleTemplate* storageTemplate = storageModule->Template<StorageSlotModuleTemplate>();
        AssertRelease(storageTemplate != nullptr);

        const PositionModule* positionModule = positionAccessor[unit];
        AssertRelease(positionModule != nullptr);

        const mat4 positionMatrix = Translation(positionModule->GetPosition3D());
        DrawCicle(GameplayConstants::Storage::SelectedColor, storageTemplate->RadiusOfEffect(), positionMatrix);
    }

    storageSlotAccessor.UnlockIFN();
    positionAccessor.UnlockIFN();
}

} // namespace ECSEngine