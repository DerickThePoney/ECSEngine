#include "stdafx.h"

#include "ColonyFeedbackSystem.h"

#include "CircularBuildingGrid.h"
#include "CircularGridAccessor.h"
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/InputManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "GameplayConstants.h"
#include "GameplayFeedbackDrawer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{

ColonyFeedbackSystem::ColonyFeedbackSystem()
{
}

void ColonyFeedbackSystem::VirtualUpdate()
{
    ModuleSystem::VirtualUpdate();

    if (CircularBuildingGrid::HasInstance())
    {
        u32 FCameraId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(FCameraId);
        const uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
        const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();

        bool foundPos = false;
        vec3 mouseWorldPosition = GetWorldPositionFromScreenPosition(*camera, aspectRatio, windowSize, Input::GetMousePosition(), foundPos);
        CircularGridAccessor accessor = CircularBuildingGrid::Instance().GetAccessorForWorldPosition(mouseWorldPosition);

        CircularBuildingGrid::Instance().DrawFeedback();

        if (accessor.Valid())
        {
            GameplayFeedbackDrawer::Instance().AddAABB(accessor.CellPosition() + vec3(-0.1f), accessor.CellPosition() + vec3(0.1f),
                  (accessor.IsFree(1)) ? 0xFF00FF00 : 0xFF0000FF, mat4::Identity(), true);
            GameplayFeedbackDrawer::Instance().AddAABB(mouseWorldPosition + vec3(-0.1f), mouseWorldPosition + vec3(0.1f), 0xFF00FFFF, mat4::Identity(), true);
        }
        else
        {
            GameplayFeedbackDrawer::Instance().AddAABB(mouseWorldPosition + vec3(-0.1f), mouseWorldPosition + vec3(0.1f), 0xFF0000FF, mat4::Identity(), true);
        }
    }
}

} // namespace ECSEngine
