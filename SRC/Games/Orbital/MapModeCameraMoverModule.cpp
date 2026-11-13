#include "stdafx.h"

#include "MapModeCameraMoverModule.h"

#include "Common/CameraManager.h"
#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::MapModeCameraMoverModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::MapModeCameraMoverModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(MapModeCameraMoverModule, MapModeCameraMoverModuleTemplate);

Module* MapModeCameraMoverModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<MapModeCameraMoverModule>(this, parUnitId, parParameters);
}

void MapModeCameraMoverModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_STRING("Camera name", FCameraName, false, "");
    EDITOR_PROPERTY_WITH_LIMITS("Initial Camera Distance", FInitialDistance, 50.f, 50000.f);
    EDITOR_PROPERTY_ANGLE("Initial Pitch", FInitialPitch, -89.f, 89.f);
    EDITOR_PROPERTY_ANGLE("Initial Yaw", FInitialYaw, -180.f, 180.f);
}

IMPLEMENT_SAVELOAD_ABILITIES(MapModeCameraMoverModule);
template<typename Chunk, bool isWriting>
void MapModeCameraMoverModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
    parChunk & FDistance;
    parChunk & FPitch;
    parChunk & FYaw;
    parChunk & FTangent;
    parChunk & FOrbitInitialised;
    parChunk & FDistanceFitted;

    if (!isWriting)
    {
        const MapModeCameraMoverModuleTemplate* moduleTemplate = Template<MapModeCameraMoverModuleTemplate>();
        AssertRelease(moduleTemplate != nullptr);

        FCamId = CameraManager::Instance().CreateCameraIFN(moduleTemplate->CameraName());
        AssertRelease(FCamId != -1);
    }
}

MapModeCameraMoverModule::MapModeCameraMoverModule()
    : Module()
{
}

MapModeCameraMoverModule::~MapModeCameraMoverModule()
{
}

void MapModeCameraMoverModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    const MapModeCameraMoverModuleTemplate* const Temp = Template<MapModeCameraMoverModuleTemplate>();
    FDistance = Temp->InitialDistance();
    FPitch = Temp->InitialPitch();
    FYaw = Temp->InitialYaw();

    FTargetedPlanet = parParameters.Get_IFP<ModuleParameters::TargetId>(EntityId());

    FCamId = CameraManager::Instance().CreateCameraIFN(Temp->CameraName());
    AssertRelease(FCamId != -1);
}

} // namespace ECSEngine
