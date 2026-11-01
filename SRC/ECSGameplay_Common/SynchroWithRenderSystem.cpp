#include "stdafx.h"

#include "SynchroWithRenderSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/WorldIds.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "RenderingCore/GFXKeyHelper.h"
#include "RenderingCore/GFXRepresentationProxy.h"

namespace ECSEngine
{
SynchroWithRenderSystem::SynchroWithRenderSystem()
    : parent_type()
{
    RegisterDependency<ApparenceModule>(EEntityWorlds::STANDARD);
    RegisterDependency<PositionModule>(EEntityWorlds::STANDARD);
    RegisterDependency<OrientationModule>(EEntityWorlds::STANDARD);

    if (WorldManager::Instance().GetWorldIFP(EEntityWorlds::RESOURCE_PROD) != nullptr)
    {
        RegisterDependency<ApparenceModule>(EEntityWorlds::RESOURCE_PROD);
        RegisterDependency<PositionModule>(EEntityWorlds::RESOURCE_PROD);
        RegisterDependency<OrientationModule>(EEntityWorlds::RESOURCE_PROD);
    }
}

SynchroWithRenderSystem::~SynchroWithRenderSystem()
{
}

template<EEntityWorlds world>
void UpdateObjectsForRendering()
{
    ModuleAccessor<ApparenceModule> apparenceController(world);
    ModuleAccessor<PositionModule> positionController(world);
    ModuleAccessor<OrientationModule> orientationController(world);

    foreachitem(apparenceModule, apparenceController)
    {
        const EntityId& unitId = apparenceModule.UnitId();

        const PositionModule* positionModule = positionController[unitId];
        AssertRelease(positionModule != nullptr);

        const OrientationModule* orientationModule = orientationController[unitId];
        AssertRelease(orientationModule != nullptr);

        Rendering::GFXRepresentationProxy* proxy = apparenceModule.Proxy();
        proxy->PushMessage<vec3>(GFXKeyHelper::Instance().Position, positionModule->GetPosition3D(), TimeManager::CurrentGameplayTime());
        proxy->PushMessage<quat>(GFXKeyHelper::Instance().Orientation, orientationModule->GetOrientation(), TimeManager::CurrentGameplayTime());
    }
}

void SynchroWithRenderSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    UpdateObjectsForRendering<EEntityWorlds::STANDARD>();
    if (WorldManager::Instance().GetWorldIFP(EEntityWorlds::RESOURCE_PROD) != nullptr)
        UpdateObjectsForRendering<EEntityWorlds::RESOURCE_PROD>();
}

} // namespace ECSEngine
