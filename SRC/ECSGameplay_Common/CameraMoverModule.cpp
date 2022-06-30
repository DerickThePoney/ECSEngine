#include "stdafx.h"

#include "CameraMoverModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/CameraManager.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::CameraMoverModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::CameraMoverModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(CameraMoverModule, CameraMoverModuleTemplate);

Module* CameraMoverModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<CameraMoverModule>(this, parUnitId, parParameters);
}

void CameraMoverModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_STRING("Camera name", FCameraName, false, "");
    EDITOR_PROPERTY_SIMPLE("Camera Max Speed", FCameraMaxSpeed);
    EDITOR_PROPERTY_SIMPLE("Camera Acceleration", FCameraAcceleration);
    EDITOR_PROPERTY_SIMPLE("Camera Rotation Max Speed", FCameraRotationMaxSpeed);
    EDITOR_PROPERTY_SIMPLE("Camera Rotation Acceleration", FCameraRotationAcceleration);
}

void CameraMoverModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const CameraMoverModuleTemplate* moduleTemplate = Template<CameraMoverModuleTemplate>();
    AssertRelease(moduleTemplate != nullptr);

    FCamId = CameraManager::Instance().CreateCameraIFN(moduleTemplate->CameraName());
    AssertRelease(FCamId != -1);
}

IMPLEMENT_SAVELOAD_ABILITIES(CameraMoverModule);
template<typename Chunk, bool isWriting>
void CameraMoverModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);

    parChunk& FCurrentSpeed;
    parChunk& FCurrentRotationSpeed;

    if (!isWriting)
    {
        const CameraMoverModuleTemplate* moduleTemplate = Template<CameraMoverModuleTemplate>();
        AssertRelease(moduleTemplate != nullptr);

        FCamId = CameraManager::Instance().CreateCameraIFN(moduleTemplate->CameraName());
        AssertRelease(FCamId != -1);
    }
}

} // namespace ECSEngine
