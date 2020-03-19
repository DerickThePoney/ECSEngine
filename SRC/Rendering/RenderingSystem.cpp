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
#include "RenderingCore/GLFWDisplayWindowHandler.h"
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
bgfx::ProgramHandle kProgram;
bgfx::ProgramHandle kProgramInstancing;
bgfx::UniformHandle kUniform;
void RenderingSystem::VirtualInit()
{
    parent_type::VirtualInit();

    kProgram = ECSEngine::Rendering::LoadProgram("Shaders\\Perso\\", "VertexColor");
    kProgramInstancing = ECSEngine::Rendering::LoadProgram("Shaders\\Perso\\", "VertexColorInstancing");

    kUniform = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
    const glm::vec3 at = { 0.0f, 0.0f, 0.0f };
    const glm::vec3 eye = { 0.0f, 0.0f, -5.0f };

    glm::quat orientation = glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));

    u32 CamId = CameraManager::Instance().CreateCamera();
    c = CameraManager::Instance().GetCamera(CamId);
    AlwaysCheckedAssert(!c.expired());
    std::shared_ptr<Camera> cshared = c.lock();
    cshared->Init(eye, orientation, glm::radians(60.0f), 0.1f, 100.0f);
}

void RenderingSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    ModuleAccessor<ApparenceModule> apparenceController;
    ModuleAccessor<PositionModule> positionController;
    ModuleAccessor<OrientationModule> orientationController;

    const float timepoint = TimeManager::DurationSinceStartRealTime();
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    const glm::vec3 at = { 0.0f, 0.0f, 0.0f };
    const glm::vec3 eye = { 0.0f, 0.0f, -5.0f };
    AlwaysCheckedAssert(!c.expired());
    std::shared_ptr<Camera> cshared = c.lock();
    cshared->SetPosition(glm::vec3(glm::cos(timepoint / 10.f), glm::sin(timepoint / 10.f), -5.0f));

    glm::mat4 view = cshared->GetWorldViewMatrix();
    glm::mat4 proj = cshared->GetProjectionMatrix(float(windowSize.x) / float(windowSize.y));
    bgfx::setViewTransform(0, &view[0][0], &proj[0][0]);

    const float sinTime = 0.5f * (sin(3.14f * timepoint / 10.f) + 1);
    float uniformVal[4] = { sinTime, sinTime, sinTime, sinTime };
    bgfx::setUniform(kUniform, &uniformVal);

    if (!Rendering::BGFXRenderer::Instance().IsInstancingEnabled())
    {
        foreachitem(apparenceModule, apparenceController)
        {
            const EntityId& unitId = apparenceModule.UnitId();
            Rendering::MeshHandle meshHandle = apparenceModule.GetMeshHandle();

            Rendering::IMesh* mesh = Rendering::MeshManager::Instance().GetMesh(meshHandle);

            bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
            bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());

            const PositionModule* positionModule = positionController[unitId];
            AssertRelease(positionModule != nullptr);

            const OrientationModule* orientationModule = orientationController[unitId];
            AssertRelease(orientationModule != nullptr);
            glm::mat4 mtx = glm::translate(glm::vec3(positionModule->GetPosition3D()));
            mtx = mtx * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * (glm::mat4)orientationModule->GetOrientation();

            bgfx::setTransform(&mtx[0][0]);
            bgfx::submit(0, kProgram);
        }
    }
    else
    {
        // 80 bytes stride = 64 bytes for 4x4 matrix + 16 bytes for RGBA color.
        const uint16_t instanceStride = 64;
        // 11x11 cubes
        const uint32_t numInstances = apparenceController.GetSize();

        if (numInstances > 0 && numInstances == bgfx::getAvailInstanceDataBuffer(numInstances, instanceStride))
        {
            bgfx::InstanceDataBuffer idb;
            bgfx::allocInstanceDataBuffer(&idb, numInstances, instanceStride);

            uint8_t* data = idb.data;

            u32 i = 0;

            foreachitem(apparenceModule, apparenceController)
            {
                const EntityId& unitId = apparenceModule.UnitId();
                if (i == 0)
                {
                    Rendering::MeshHandle meshHandle = apparenceModule.GetMeshHandle();

                    Rendering::IMesh* mesh = Rendering::MeshManager::Instance().GetMesh(meshHandle);

                    bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
                    bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());
                }

                const PositionModule* positionModule = positionController[unitId];
                AssertRelease(positionModule != nullptr);

                const OrientationModule* orientationModule = orientationController[unitId];
                AssertRelease(orientationModule != nullptr);
                glm::mat4 mtx = glm::translate(glm::vec3(positionModule->GetPosition3D()));
                mtx = mtx * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * (glm::mat4)orientationModule->GetOrientation();
                memcpy(data, &mtx, sizeof(mtx));
                data += instanceStride;
                ++i;
            }

            // Set instance data buffer.
            bgfx::setInstanceDataBuffer(&idb);

            // Set render states.
            bgfx::setState(BGFX_STATE_DEFAULT);

            // Submit primitive for rendering to view 0.
            bgfx::submit(0, kProgramInstancing);
        }
    }
}

void RenderingSystem::VirtualDestroy()
{
    parent_type::VirtualDestroy();
    bgfx::destroy(kUniform);
    bgfx::destroy(kProgramInstancing);
    bgfx::destroy(kProgram);
}

} // namespace ECSEngine