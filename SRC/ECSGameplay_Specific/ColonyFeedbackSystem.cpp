#include "stdafx.h"

#include "ColonyFeedbackSystem.h"

#include "CircularBuildingGrid.h"
#include "ColonyTraitsModule.h"
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

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
        u32 FCameraId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
        const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
        const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();

        bool foundPos = false;
        glm::vec3 mouseWorldPosition = GetWorldPositionFromScreenPosition(*camera, aspectRatio, windowSize, Input::GetMousePosition(), foundPos);

        CircularGridAccessor accessor = CircularBuildingGrid::Instance().GetAccessorForWorldPosition(mouseWorldPosition);

        CircularBuildingGrid::Instance().DrawFeedback();

        if (accessor.Valid())
        {
            GameplayFeedbackDrawer::Instance().AddAABB(accessor.CellPosition() + glm::vec3(-0.1f), accessor.CellPosition() + glm::vec3(0.1f),
                  (accessor.IsFree()) ? 0xFF00FF00 : 0xFF0000FF, glm::identity<glm::mat4>(), true);
            GameplayFeedbackDrawer::Instance().AddAABB(mouseWorldPosition + glm::vec3(-0.1f), mouseWorldPosition + glm::vec3(0.1f), 0xFF00FFFF, glm::identity<glm::mat4>(), true);
        }
        else
        {
            GameplayFeedbackDrawer::Instance().AddAABB(mouseWorldPosition + glm::vec3(-0.1f), mouseWorldPosition + glm::vec3(0.1f), 0xFF0000FF, glm::identity<glm::mat4>(), true);
        }
    }
}

} // namespace ECSEngine
