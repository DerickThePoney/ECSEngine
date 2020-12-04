#include "stdafx.h"

#include "RenderingSystem.h"

#include "Common/CameraManager.h"
#include "Common/Frustum.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/BGFXRenderingUtils.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/Mesh.h"
#include "RenderingCore/MeshCuller.h"
#include "RenderingCore/MeshManager.h"
#include "RenderingCore/Texture.h"
#include "RenderingCore/TextureBank.h"
#include "RenderingCore/TextureDescriptor.h"
#include "RenderingCore/TexturesManager.h"
#include "bx/bx.h"

namespace ECSEngine
{
RenderingSystem::RenderingSystem()
    : parent_type()
    , FDrawBuffer(nullptr)
    , FCamId(-1)
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

RenderingSystem::~RenderingSystem()
{
}

void RenderingSystem::VirtualInit()
{
    parent_type::VirtualInit();

    FCamId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");

    FDrawBuffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::GEOMETRY_PASS);
}

template<Worlds::Type world>
void RenderObjects(Rendering::DrawCommandBuffer* parCommandBuffer, const Frustum& parFrustum)
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
        glm::mat4 mtx = glm::translate(glm::vec3(positionModule->GetPosition3D()));
        mtx = mtx * (glm::mat4)orientationModule->GetOrientation();

        if (Rendering::MeshFrustumCulling::CullApparenceModule(apparenceModule, mtx, parFrustum))
        {
            parCommandBuffer->DrawMesh(meshHandle, materialHandle, mtx);
        }
    }
}

void RenderingSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    AssertRelease(FDrawBuffer != nullptr);
    FDrawBuffer->clear();

    const float timepoint = TimeManager::DurationSinceStartRealTime();
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const float aspectRatio = float(windowSize.x) / float(windowSize.y);

    Camera* c = CameraManager::Instance().GetCamera(FCamId);
    AssertRelease(c != nullptr);

    Frustum frustum;
    frustum.InitFromCamera(*c, aspectRatio);

    glm::mat4 view = c->GetWorldViewMatrix();
    glm::mat4 proj = c->GetProjectionMatrix(aspectRatio);
    FDrawBuffer->SetViewTranform(view, proj);

    RenderObjects<Worlds::STANDARD>(FDrawBuffer, frustum);
    RenderObjects<Worlds::RESOURCE_PROD>(FDrawBuffer, frustum);
    RenderObjects<Worlds::PEONS>(FDrawBuffer, frustum);

    if (1) { }
    else
    {
        //// 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
        // const uint16_t instanceStride = 64;
        //// 11x11 cubes
        // const uint32_t numInstances = apparenceController.GetSize();

        // if (numInstances > 0 && numInstances == bgfx::getAvailInstanceDataBuffer(numInstances, instanceStride))
        //{
        //    bgfx::InstanceDataBuffer idb;
        //    bgfx::allocInstanceDataBuffer(&idb, numInstances, instanceStride);

        //    uint8_t* data = idb.data;

        //    u32 i = 0;

        //    foreachitem(apparenceModule, apparenceController)
        //    {
        //        const EntityId& unitId = apparenceModule.UnitId();
        //        if (i == 0)
        //        {
        //            Rendering::MeshHandle meshHandle = apparenceModule.GetMeshHandle();

        //            Rendering::IMesh* mesh = Rendering::MeshManager::Instance().GetMesh(meshHandle);

        //            bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
        //            bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());
        //        }

        //        const PositionModule* positionModule = positionController[unitId];
        //        AssertRelease(positionModule != nullptr);

        //        const OrientationModule* orientationModule = orientationController[unitId];
        //        AssertRelease(orientationModule != nullptr);
        //        glm::mat4 mtx = glm::translate(glm::vec3(positionModule->GetPosition3D()));
        //        mtx = mtx * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * (glm::mat4)orientationModule->GetOrientation();
        //        memcpy(data, &mtx, sizeof(mtx));
        //        data += instanceStride;
        //        ++i;
        //    }

        //    // Set instance data buffer.
        //    bgfx::setInstanceDataBuffer(&idb);

        //    // Set render states.
        //    bgfx::setState(BGFX_STATE_DEFAULT);

        //    // Submit primitive for rendering to view 0.
        //    bgfx::submit(0, kProgramInstancing);
        //}
    }

    FDrawBuffer->Submit();
}

void RenderingSystem::VirtualDestroy()
{
    parent_type::VirtualDestroy();
    Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(FDrawBuffer);
    /*bgfx::destroy(kProgramInstancing);*/
}

} // namespace ECSEngine