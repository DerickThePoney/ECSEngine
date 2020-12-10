#include "stdafx.h"

#include "SynchroWithRenderSystem.h"

#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
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
    RegisterDepency<ApparenceModule>(Worlds::STANDARD);
    RegisterDepency<PositionModule>(Worlds::STANDARD);
    RegisterDepency<OrientationModule>(Worlds::STANDARD);

    RegisterDepency<ApparenceModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<PositionModule>(Worlds::RESOURCE_PROD);
    RegisterDepency<OrientationModule>(Worlds::RESOURCE_PROD);

    RegisterDepency<ApparenceModule>(Worlds::PEONS);
    RegisterDepency<PositionModule>(Worlds::PEONS);
    RegisterDepency<OrientationModule>(Worlds::PEONS);
}

SynchroWithRenderSystem::~SynchroWithRenderSystem()
{
}

template<Worlds::Type world>
void UpdateObjectsForRendering()
{
    ModuleAccessor<ApparenceModule> apparenceController(world);
    ModuleAccessor<PositionModule> positionController(world);
    ModuleAccessor<OrientationModule> orientationController(world);

    foreachitem(apparenceModule, apparenceController)
    {
        const EntityId& unitId = apparenceModule.UnitId();
        const Rendering::MeshHandle& meshHandle = apparenceModule.GetMeshHandle();
        const Rendering::MaterialInstanceHandle& materialHandle = apparenceModule.GetMaterialHandle();

        const PositionModule* positionModule = positionController[unitId];
        AssertRelease(positionModule != nullptr);

        const OrientationModule* orientationModule = orientationController[unitId];
        AssertRelease(orientationModule != nullptr);

        Rendering::GFXRepresentationProxy* proxy = apparenceModule.Proxy();
        proxy->PushMessage<glm::vec3>(GFXKeyHelper::Instance().Position, positionModule->GetPosition3D(), TimeManager::FrameStartTime());
        proxy->PushMessage<glm::quat>(GFXKeyHelper::Instance().Orientation, orientationModule->GetOrientation(), TimeManager::FrameStartTime());
    }
}

void SynchroWithRenderSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    UpdateObjectsForRendering<Worlds::STANDARD>();
    UpdateObjectsForRendering<Worlds::RESOURCE_PROD>();
    UpdateObjectsForRendering<Worlds::PEONS>();
}

} // namespace ECSEngine