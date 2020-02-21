#include "stdafx.h"

#include "RenderingSystem.h"

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
#include "bx/math.h"

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
bgfx::UniformHandle kUniform;
void RenderingSystem::VirtualInit()
{
    parent_type::VirtualInit();

    kProgram = ECSEngine::Rendering::LoadProgram("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\shaders\\Perso\\", "VertexColor");

    kUniform = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
}

void RenderingSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    ModuleAccessor<ApparenceModule> apparenceController;
    ModuleAccessor<PositionModule> positionController;
    ModuleAccessor<OrientationModule> orientationController;

    const float timepoint = TimeManager::DurationSinceStartRealTime();
    const glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxProj(proj, 60.0f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);

    const float sinTime = 0.5f * (sin(3.14f * timepoint / 10.f) + 1);
    float uniformVal[4] = { sinTime, sinTime, sinTime, sinTime };
    bgfx::setUniform(kUniform, &uniformVal);

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
        float mtx[16];
        const glm::aligned_vec3 position = positionModule->GetPosition3D();
        const glm::vec3 yawPitchRoll = orientationModule->GetOrientationAsYawPitchRoll();
        bx::mtxSRT(mtx, 0.1f, 0.1f, 0.1f, yawPitchRoll.x, yawPitchRoll.y, yawPitchRoll.z, position.x, position.y, position.z);
        /*bx::mtxRotateXY(mtx, timepoint, timepoint);*/
        bgfx::setTransform(mtx);
        bgfx::submit(0, kProgram);
    }
}

void RenderingSystem::VirtualDestroy()
{
    parent_type::VirtualDestroy();
    bgfx::destroy(kUniform);
    bgfx::destroy(kProgram);
}

} // namespace ECSEngine