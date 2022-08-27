#include "stdafx.h"

#include "PlaceBuildingMousePolicy.h"

#include "BuildingGridOccupancyModule.h"
#include "CircularBuildingGrid.h"
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/GenericMessageIdentifiers.h"
#include "Common/GenericMessageManager.h"
#include "Common/InputManager.h"
#include "Common/TimeManager.h"
#include "ConstructBuildingMessage.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/WorldIds.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "RenderingCore/GFXKeyHelper.h"
#include "RenderingCore/GFXRepresentationInitialiser.h"
#include "RenderingCore/GFXRepresentationProxy.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

namespace ECSEngine
{

PlaceBuildingMousePolicy::PlaceBuildingMousePolicy()
{
    FValidateInputCommand.FMouseButton = MouseButtons::MOUSE_BUTTON_1;
    FValidateInputCommand.FInputType = EInputType::RELEASED;

    FShiftedValidationCommand.FMouseButton = MouseButtons::MOUSE_BUTTON_1;
    FShiftedValidationCommand.FInputType = EInputType::RELEASED;
    FShiftedValidationCommand.FShift = true;
}

void PlaceBuildingMousePolicy::SetupMousePolicy(const std::string& parBuildingTemplateName)
{
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(parBuildingTemplateName);
    AssertRelease(FTemplate != nullptr);
    AssertRelease(FTemplate->GetWorldId() == EEntityWorlds::BUILDINGS);
}

void PlaceBuildingMousePolicy::VirtualActivate()
{
    parent_type::VirtualActivate();

    AssertRelease(FTemplate != nullptr);
    const ApparenceModuleTemplate* apparenceTemplate = (const ApparenceModuleTemplate*)FTemplate->GetModuleTemplate<ApparenceModule>();
    AlwaysCheckedAssert(apparenceTemplate != nullptr);

    if (apparenceTemplate == nullptr)
    {
        Deactivate();
        return;
    }

    const BuildingGridOccupancyModuleTemplate* occupancyTemplate = (const BuildingGridOccupancyModuleTemplate*)FTemplate->GetModuleTemplate<BuildingGridOccupancyModule>();
    AssertRelease(occupancyTemplate != nullptr);
    FCellOccupancy = occupancyTemplate->CellsOccupancy();

    FBuildingProxy = new Rendering::GFXRepresentationProxy();
    Rendering::GFXRepresentationInitialiser init;
    init.FPosition = GetMouseWorldPosition();
    init.FOrientation = glm::quat(1.f, 0.f, 0.f, 0.f);
    init.HasCarier = true;
    init.FCurrentTime = TimeManager::FrameStartTime();
    AssertRelease(!apparenceTemplate->GFXRepresentationDescriptorName().empty());
    init.FRepresentationDescriptor = apparenceTemplate->GFXRepresentationDescriptorName();
    init.HasVisuals = true;
    FBuildingProxy->Initialise(init);
}

void PlaceBuildingMousePolicy::VirtualDeactivate()
{
    parent_type::VirtualDeactivate();

    FTemplate = nullptr;
    delete FBuildingProxy;
}

void PlaceBuildingMousePolicy::VirtualUpdate()
{
    parent_type::VirtualUpdate();
    AssertRelease(FBuildingProxy != nullptr);

    if (CircularBuildingGrid::HasInstance())
    {
        // TODO CHECK RESOURCES !

        bool foundPos = false;
        glm::vec3 mouseWorldPosition = GetMouseWorldPosition();

        CircularGridAccessor accessor = CircularBuildingGrid::Instance().GetAccessorForWorldPosition(mouseWorldPosition);

        if (accessor.Valid() && accessor.IsFree(FCellOccupancy))
        {
            FBuildingProxy->PushMessage<bool>(GFXKeyHelper::Instance().Visible, true, TimeManager::FrameStartTime());
            FBuildingProxy->PushMessage<glm::vec3>(GFXKeyHelper::Instance().Position, accessor.CellPosition(), TimeManager::FrameStartTime());

            const bool validateNormal = FValidateInputCommand.Evaluate();
            const bool validateShifted = FShiftedValidationCommand.Evaluate();
            if (validateNormal || validateShifted)
            {
                ConstructBuildingMessage* message = new ConstructBuildingMessage();
                message->FTemplateName = FTemplate->GetName();
                message->FPosition = accessor.CellPosition();
                GenericMessageManager::Instance().PushMessage<GenericMessageId::PLACE_BUILDING, ConstructBuildingMessage>(message);
                accessor.SetOccupied(true, FCellOccupancy);
                if (validateNormal && !validateShifted)
                    Deactivate();
                return;
            }
        }
        else
        {
            FBuildingProxy->PushMessage<bool>(GFXKeyHelper::Instance().Visible, false, TimeManager::FrameStartTime());
        }
    }
}

glm::vec3 PlaceBuildingMousePolicy::GetMouseWorldPosition() const
{
    u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    Camera* camera = CameraManager::Instance().GetCamera(camId);
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio();
    bool dummy = false;
    return GetWorldPositionFromScreenPosition(
          *camera, Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio(), Rendering::GLFWDisplayWindowHandler::Instance().GetSize(), Input::GetMousePosition(), dummy);
}

} // namespace ECSEngine
