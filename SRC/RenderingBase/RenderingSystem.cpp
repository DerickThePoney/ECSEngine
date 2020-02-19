#include "stdafx.h"

#include "RenderingSystem.h"

#include "BGFXRenderer.h"
#include "BGFXRenderingUtils.h"
#include "DisplayWindow.h"
#include "ECSBase/ModuleAccessor.h"
#include "ECSGameplay_Base/ApparenceModule.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "bx/bx.h"
#include "bx/math.h"

namespace ECSEngine
{
RenderingSystem::RenderingSystem()
    : parent_type()
{
    RegisterDepency<ApparenceModule>(Worlds::STANDARD);
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

    FStart = std::chrono::high_resolution_clock::now();
}

void RenderingSystem::VirtualUpdate()
{
    parent_type::VirtualUpdate();

    ModuleAccessor<ApparenceModule> apparenceController;

    auto this_tick = std::chrono::high_resolution_clock::now();
    const float timepoint = (float)(this_tick - FStart).count() / 1000000000.0f;
    const glm::uvec2 windowSize = Rendering::DisplayWindow::Instance().GetSize();

    const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
    const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
    float view[16];
    bx::mtxLookAt(view, eye, at);
    float proj[16];
    bx::mtxProj(proj, 60.0f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(0, view, proj);

    foreachitem(apparenceModule, apparenceController)
    {
        Rendering::MeshHandle meshHandle = apparenceModule.GetMeshHandle();

        Rendering::IMesh* mesh = Rendering::MeshManager::Instance().GetMesh(meshHandle);

        bgfx::setVertexBuffer(0, mesh->GetVertexBufferHandle());
        bgfx::setIndexBuffer(mesh->GetIndexBufferHandle());

        const float sinTime = 0.5f * (sin(3.14f * timepoint / 10.f) + 1);
        float uniformVal[4] = { sinTime, sinTime, sinTime, sinTime };
        bgfx::setUniform(kUniform, &uniformVal);

        float mtx[16];
        bx::mtxRotateXY(mtx, timepoint, timepoint);
        bgfx::setTransform(mtx);
        bgfx::submit(0, kProgram);
    }

    Rendering::BGFXRenderer::Instance().RenderFrame();
}

void RenderingSystem::VirtualDestroy()
{
    parent_type::VirtualDestroy();
    bgfx::destroy(kUniform);
    bgfx::destroy(kProgram);
}

} // namespace ECSEngine