
#include "stdafx.h"

#include "RocketCameraMoverModule.h"

#include "Common/CameraManager.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::RocketCameraMoverModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::RocketCameraMoverModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(RocketCameraMoverModule, RocketCameraMoverModuleTemplate);

Module* RocketCameraMoverModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<RocketCameraMoverModule>(this, parUnitId, parParameters);
}

void RocketCameraMoverModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_STRING("Camera name", FCameraName, false, "");
    EDITOR_PROPERTY_WITH_LIMITS("Initial Camera Distance", FInitialDistance, 8.f, 8000.f);
    EDITOR_PROPERTY_ANGLE("Initial Pitch", FInitialPitch, -80.f, 80.f);
    EDITOR_PROPERTY_ANGLE("Initial Yaw", FInitialYaw, -180.f, 180.f);
}

IMPLEMENT_SAVELOAD_ABILITIES(RocketCameraMoverModule);
template<typename Chunk, bool isWriting>
void RocketCameraMoverModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
    parChunk & FDistance;
    parChunk & FPitch;
    parChunk & FYaw;
    parChunk & FTangent;
    parChunk & FOrbitInitialised;

    if (!isWriting)
    {
        const RocketCameraMoverModuleTemplate* moduleTemplate = Template<RocketCameraMoverModuleTemplate>();
        AssertRelease(moduleTemplate != nullptr);

        FCamId = CameraManager::Instance().CreateCameraIFN(moduleTemplate->CameraName());
        AssertRelease(FCamId != -1);
    }
}

RocketCameraMoverModule::RocketCameraMoverModule()
    : Module()
{
}

RocketCameraMoverModule::~RocketCameraMoverModule()
{
}

void RocketCameraMoverModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const RocketCameraMoverModuleTemplate* const Temp = Template<RocketCameraMoverModuleTemplate>();
    FDistance = Temp->InitialDistance();
    FPitch = Temp->InitialPitch();
    FYaw = Temp->InitialYaw();

    FTargetedRocket = parParameters.Get_IFP<ModuleParameters::TargetId>(EntityId());
    AssertRelease(FTargetedRocket.Valid());

    FCamId = CameraManager::Instance().CreateCameraIFN(Temp->CameraName());
    AssertRelease(FCamId != -1);
}

} // namespace ECSEngine
