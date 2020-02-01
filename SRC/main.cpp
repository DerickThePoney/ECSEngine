#include "stdafx.h"

#include "../glfw-3.3.bin.WIN64/include/GLFW/glfw3.h"
#include "BGFXRenderer.h"
#include "BGFXRenderingUtils.h"
#include "DisplayWindow.h"
#include "EntityId.h"
#include "EntityTemplateManager.h"
#include "EntityWorld.h"
#include "Module.h"
#include "ModuleAccessor.h"
#include "ModuleController.h"
#include "ModuleId.h"
#include "ModuleParameters.h"
#include "MovementSystem.h"
#include "PositionModule.h"
#include "Resource.h"
#include "ResourceCache.h"
#include "ResourceFileDirectoryView.h"
#include "ResourceHandle.h"
#include "WorldManager.h"
#include "bgfx/embedded_shader.h"
#include "bx/math.h"

struct PosColorVertex
{
    float x;
    float y;
    float z;
    uint32_t abgr;
};

static PosColorVertex cubeVertices[] = {
    { -1.0f, 1.0f, 1.0f, 0xff000000 },
    { 1.0f, 1.0f, 1.0f, 0xff0000ff },
    { -1.0f, -1.0f, 1.0f, 0xff00ff00 },
    { 1.0f, -1.0f, 1.0f, 0xff00ffff },
    { -1.0f, 1.0f, -1.0f, 0xffff0000 },
    { 1.0f, 1.0f, -1.0f, 0xffff00ff },
    { -1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] = {
    0,
    1,
    2,
    1,
    3,
    2,
    4,
    6,
    5,
    5,
    6,
    7,
    0,
    2,
    4,
    4,
    2,
    6,
    1,
    5,
    3,
    5,
    7,
    3,
    0,
    4,
    1,
    4,
    5,
    1,
    2,
    3,
    6,
    6,
    3,
    7,
};

void TestFunction()
{
    std::cout << "EntityID size: " << sizeof(ECSEngine::EntityId) << "\n";
    std::cout << "glm::vec3 size: " << sizeof(glm::aligned_vec3) << "\n";

    ECSEngine::ModuleTraits<ECSEngine::PositionModule> b;

    constexpr u32 res = b.GetModuleId();

    ECSEngine::ModuleController<ECSEngine::PositionModule> controller;

    controller.AllocateForEntity(ECSEngine::EntityId(0, 0));

    std::cout << "PositionModule Id size: " << ECSEngine::ModuleTraits<ECSEngine::PositionModule>::GetModuleId() << "\n";

    controller.DeallocateForEntity(ECSEngine::EntityId(0, 0));

    ECSEngine::WorldManager::CreateIFP();
    AssertRelease(ECSEngine::WorldManager::HasInstance());
    ECSEngine::WorldManager& worldManagerInstance = ECSEngine::WorldManager::Instance();
    worldManagerInstance.Init();

    ECSEngine::EntityWorld& world = worldManagerInstance.GetWorld(ECSEngine::Worlds::STANDARD);

    ECSEngine::EntityTemplateManager::CreateIFP();
    AssertRelease(ECSEngine::EntityTemplateManager::HasInstance());
    ECSEngine::EntityTemplate* newTemplate = ECSEngine::EntityTemplateManager::Instance().CreateNewEntityTemplate();
    newTemplate->SetHasModule<ECSEngine::PositionModule>();

    ECSEngine::ModuleParameters::ParameterContainer container;
    container.Set<ECSEngine::ModuleParameters::Position>(glm::vec3(1.5f, 2.5f, 3.5f));

    ECSEngine::EntityId unitID = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
    ECSEngine::EntityId unitID2 = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Lock();
    ECSEngine::ModuleAccessor<ECSEngine::PositionModule> moduleAccessor;

    ECSEngine::PositionModule* positionModule = moduleAccessor[unitID];
    const ECSEngine::PositionModule* positionModuleConst = moduleAccessor[unitID];

    AssertRelease(positionModule != nullptr && positionModule == positionModuleConst);
    foreachitem(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    foreachitemconst(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    reverseforeachitem(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }
    reverseforeachitemconst(positionMod, moduleAccessor) { std::cout << "Position module " << &positionMod << "\t" << glm::to_string(positionMod.GetPosition3D()) << std::endl; }

    dynamic_cast<ECSEngine::ModuleController<ECSEngine::PositionModule>*>(world.GetControllerIFP<ECSEngine::PositionModule>())->Unlock();

    ECSEngine::EntityId unitID3 = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
    ECSEngine::EntityId unitID4 = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
    ECSEngine::EntityId unitID5 = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
    ECSEngine::EntityId unitID6 = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);

    ECSEngine::MovementSystem movementSystem;
    movementSystem.Init();

    for (int i = 0; i < 1000; ++i)
    {
        movementSystem.Update();
    }

    world.DestroyEntity(unitID);
    world.DestroyEntity(unitID2);
    world.DestroyEntity(unitID3);
    world.DestroyEntity(unitID4);
    world.DestroyEntity(unitID5);
    world.DestroyEntity(unitID6);

    worldManagerInstance.Destroy();

    worldManagerInstance.Destroy();
}

int main(int argc, char** argv)
{
    ECSEngine::ModuleParameters::InitParameterIdentifiersTraits();
    TestFunction();

    ECSEngine::GlobalResourceCache::CreateIFP();
    ECSEngine::GlobalResourceCache::Instance().FCache = new ECSEngine::ResourceCache(
          10, new ECSEngine::ResourceFileDirectoryView("D:\\Programmation\\GameEngine\\ECSEngine\\Assets"));

    if (!ECSEngine::GlobalResourceCache::Instance().FCache->Initialize())
        AssertNotReachedMsg("Unable to init the resource cache!!");

    glfwInit();

    ECSEngine::Rendering::DisplayWindow::CreateIFP();
    ECSEngine::Rendering::DisplayWindow::Instance().Init();

    ECSEngine::Rendering::BGFXRenderer::CreateIFP();
    ECSEngine::Rendering::BGFXRenderer& rendererInstance = ECSEngine::Rendering::BGFXRenderer::Instance();
    rendererInstance.Init();

    bgfx::VertexLayout pcvDecl;
    pcvDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true).end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

    bgfx::ProgramHandle program = ECSEngine::Rendering::LoadProgram("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\shaders\\Perso\\", "VertexColor");

    bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;
    auto timeElapsed = end - start;
    unsigned int counter = 0;
    while (!glfwWindowShouldClose(ECSEngine::Rendering::DisplayWindow::Instance().GetWindowHandle()))
    {
        auto this_tick = std::chrono::high_resolution_clock::now();
        const float timepoint = (float)(this_tick - start).count() / 1000000000.0f;
        const glm::uvec2 windowSize = ECSEngine::Rendering::DisplayWindow::Instance().GetSize();

        const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
        float view[16];
        bx::mtxLookAt(view, eye, at);
        float proj[16];
        bx::mtxProj(proj, 60.0f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        float mtx[16];
        bx::mtxRotateXY(mtx, timepoint, timepoint);
        bgfx::setTransform(mtx);

        float colorMult = sin(0.1f * timepoint);
        colorMult *= colorMult;
        const float color[4] = { colorMult, colorMult, colorMult, 1.f };
        bgfx::setUniform(u_color, color);

        bgfx::submit(0, program);

        rendererInstance.RenderFrame();
        glfwPollEvents();
        counter++;
        end = std::chrono::high_resolution_clock::now();
        auto timeElapsed = end - start;
    }

    rendererInstance.Shutdown();

    ECSEngine::Rendering::DisplayWindow::Instance().Shutdown();

    glfwTerminate();

    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();

    return 0;
}