#include "stdafx.h"

#include "RenderingSystem.h"

#include "Common/CameraManager.h"
#include "Common/TimeManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/BGFXRenderingUtils.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/Material.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/Mesh.h"
#include "RenderingCore/MeshManager.h"
#include "bx/bx.h"

namespace ECSEngine
{
RenderingSystem::RenderingSystem()
    : parent_type()
{
    RegisterDepency<ApparenceModule>(Worlds::STANDARD);
    RegisterDepency<PositionModule>(Worlds::STANDARD);
    RegisterDepency<OrientationModule>(Worlds::STANDARD);
}

RenderingSystem::~RenderingSystem()
{
}
Rendering::MaterialInstanceHandle kHandle;
bgfx::ProgramHandle kProgramInstancing;
const std::string& uniformName = "u_color";
void RenderingSystem::VirtualInit()
{
    parent_type::VirtualInit();

    kHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");
    kProgramInstancing = ECSEngine::Rendering::LoadProgram("Shaders\\Perso\\", "VertexColorInstancing");

    const glm::vec3 at = { 0.0f, 0.0f, 1.0f };
    const glm::vec3 eye = { 0.0f, 50.0f, 0.0f };

    glm::mat4 worldWiewMatrix = glm::lookAt(eye, at, glm::vec3(0, 0, 1.0f));

    u32 CamId = CameraManager::Instance().CreateCamera();
    c = CameraManager::Instance().GetCamera(CamId);
    AlwaysCheckedAssert(!c.expired());
    std::shared_ptr<Camera> cshared = c.lock();
    cshared->Init(worldWiewMatrix, glm::radians(60.0f), 0.1f, 300.0f);
}

void RenderingSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    Rendering::DrawCommandBuffer& commandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(0);

    // commandBuffer.DrawAABB(kHandle, glm::vec3(-1), glm::vec3(1));

    ModuleAccessor<ApparenceModule> apparenceController;
    ModuleAccessor<PositionModule> positionController;
    ModuleAccessor<OrientationModule> orientationController;

    const float timepoint = TimeManager::DurationSinceStartRealTime();
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    AlwaysCheckedAssert(!c.expired());
    std::shared_ptr<Camera> cshared = c.lock();

    const glm::vec3 at = { 0.0f, 0.0f, 1.0f };
    const glm::vec3 eye = { 0.0f, 5.0f, 0.0f };

    glm::mat4 view = cshared->GetWorldViewMatrix();
    glm::mat4 proj = cshared->GetProjectionMatrix(float(windowSize.x) / float(windowSize.y));
    commandBuffer.SetViewTranform(view, proj);

    const float sinTime = 0.5f * (sin(3.14f * timepoint / 10.f) + 1);
    glm::vec4 uniformVal = glm::vec4(sinTime, sinTime, sinTime, sinTime);
    Rendering::MaterialManager::SetVec4Uniform(uniformName, uniformVal);

    if (1) //! Rendering::BGFXRenderer::Instance().IsInstancingEnabled())
    {
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

            commandBuffer.DrawMesh(meshHandle, materialHandle, mtx);
        }
    }
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
}

void RenderingSystem::VirtualDestroy()
{
    parent_type::VirtualDestroy();
    bgfx::destroy(kProgramInstancing);
}

} // namespace ECSEngine