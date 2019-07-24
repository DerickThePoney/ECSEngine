#include "stdafx.h"

#include "../glfw-3.3.bin.WIN64/include/GLFW/glfw3.h"
#include "BGFXRenderer.h"
#include "DisplayWindow.h"
#include "EntityId.h"
#include "EntityTemplateManager.h"
#include "EntityWorld.h"
#include "ModuleAccessor.h"
#include "ModuleController.h"
#include "ModuleId.h"
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

    ECSEngine::EntityId unitID = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID2 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);

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

    ECSEngine::EntityId unitID3 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID4 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID5 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);
    ECSEngine::EntityId unitID6 = world.CreateEntityFromTemplateReturnEntityId(newTemplate);

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

bgfx::ShaderHandle loadShader(const std::string& FILENAME)
{
    const std::string baseAssetPath = "D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\";
    std::string shaderPath = "???";

    switch (bgfx::getRendererType())
    {
    case bgfx::RendererType::Noop:
    case bgfx::RendererType::Direct3D9:
        shaderPath = "shaders/dx9/";
        break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12:
        shaderPath = "shaders/dx11/";
        break;
    case bgfx::RendererType::Gnm:
        shaderPath = "shaders/pssl/";
        break;
    case bgfx::RendererType::Metal:
        shaderPath = "shaders/metal/";
        break;
    case bgfx::RendererType::OpenGL:
        shaderPath = "shaders/glsl/";
        break;
    case bgfx::RendererType::OpenGLES:
        shaderPath = "shaders/essl/";
        break;
    case bgfx::RendererType::Vulkan:
        shaderPath = "shaders/spirv/";
        break;
    }

    ECSEngine::ResourceCache* cache = ECSEngine::GlobalResourceCache::Instance().FCache;
    ECSEngine::Resource shaderResource(baseAssetPath + shaderPath + FILENAME);
    std::shared_ptr<ECSEngine::ResourceHandle> shaderDataHandle = cache->GetResourceHandle(&shaderResource);

    AssertRelease(shaderDataHandle != nullptr);
    const c8* shaderData = shaderDataHandle->Buffer();
    const u32 bufferSize = shaderDataHandle->Size();

    AssertRelease(bufferSize > 0);
    AssertRelease(shaderData != nullptr);

    const bgfx::Memory* mem = bgfx::alloc(bufferSize + 1);
    memcpy(mem->data, shaderData, bufferSize);
    mem->data[mem->size - 1] = '\0';

    return bgfx::createShader(mem);
}

int main(int argc, char** argv)
{
    TestFunction();

    ECSEngine::GlobalResourceCache::CreateIFP();
    ECSEngine::GlobalResourceCache::Instance().FCache = new ECSEngine::ResourceCache(
          10, new ECSEngine::ResourceFileDirectoryView("D:\\Programmation\\GameEngine\\ECSEngine\\Assets"));

    if (!ECSEngine::GlobalResourceCache::Instance().FCache->Initialize())
        AssertNotReachedMsg("Unable to init the resource cache!!");

    ECSEngine::Resource r("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\2D\\Textures\\element_yellow_square_glossy.png");
    std::shared_ptr<ECSEngine::ResourceHandle> handle = ECSEngine::GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);

    glfwInit();

    ECSEngine::Rendering::DisplayWindow::CreateIFP();
    ECSEngine::Rendering::DisplayWindow::Instance().Init();

    ECSEngine::Rendering::BGFXRenderer::CreateIFP();
    ECSEngine::Rendering::BGFXRenderer& rendererInstance = ECSEngine::Rendering::BGFXRenderer::Instance();
    rendererInstance.Init();

    bgfx::VertexDecl pcvDecl;
    pcvDecl.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true).end();
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

    bgfx::ShaderHandle vsh = loadShader("vs_cubes.bin");
    bgfx::ShaderHandle fsh = loadShader("fs_cubes.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vsh, fsh, true);

    unsigned int counter = 0;
    while (!glfwWindowShouldClose(ECSEngine::Rendering::DisplayWindow::Instance().GetWindowHandle()))
    {
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
        bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
        bgfx::setTransform(mtx);

        bgfx::submit(0, program);

        rendererInstance.RenderFrame();
        glfwPollEvents();
        counter++;
    }

    rendererInstance.Shutdown();

    ECSEngine::Rendering::DisplayWindow::Instance().Shutdown();

    glfwTerminate();

    return 0;
}