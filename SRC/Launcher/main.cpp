#include "stdafx.h"

//#include "../glfw-3.3.bin.WIN64/include/GLFW/glfw3.h"
//#include "ApparenceModule.h"
//#include "BGFXRenderer.h"
//#include "BGFXRenderingUtils.h"
//#include "DisplayWindow.h"
//#include "EntityId.h"
//#include "EntityTemplateManager.h"
//#include "EntityWorld.h"
//#include "IndexBuffer.h"
//#include "Mesh.h"
//#include "Module.h"
//#include "ModuleAccessor.h"
//#include "ModuleController.h"
//#include "ModuleId.h"
//#include "ModuleParameters.h"
//#include "MovementSystem.h"
#include "Common/ObjectPoolAllocator.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "ECSGameplay_Common/OrientationModule.h"
#include "ECSGameplay_Common/PositionModule.h"
#include "Rendering/RenderingSystem.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DisplayWindow.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MeshManager.h"
//#include "RenderingSystem.h"
//#include "Resource.h"
//#include "ResourceCache.h"
//#include "ResourceFileDirectoryView.h"
//#include "ResourceHandle.h"
//#include "VertexBuffer.h"
//#include "VertexLayout.h"
//#include "WorldManager.h"
//#include "bgfx/embedded_shader.h"
//#include "bx/math.h"

//#include <array>
//
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

static const u32 cubeTriList[] = {
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
    /*std::cout << "EntityID size: " << sizeof(ECSEngine::EntityId) << "\n";
    std::cout << "glm::vec3 size: " << sizeof(glm::aligned_vec3) << "\n";

    std::cout << sizeof(ECSEngine::Rendering::VertexPositionColorN<1>) << "\n";
    std::cout << sizeof(glm::vec3) << "\n";

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

    worldManagerInstance.Destroy();*/
}

// class Data
//{
// public:
//    Data()
//        : FData(nullptr)
//        , FSize(0)
//        , FBool(false)
//        , FHandle()
//    {
//        FHandle.idx = bgfx::kInvalidHandle;
//    }
//    ~Data()
//    {
//        if (FBool)
//            bgfx::destroy(FHandle);
//        FHandle.idx = bgfx::kInvalidHandle;
//        FBool = false;
//        AssertRelease(!bgfx::isValid(FHandle));
//        AssertRelease(!FBool);
//
//        if (FData != nullptr)
//        {
//            delete[] FData;
//            FData = nullptr;
//        }
//
//        FSize = 0;
//    }
//
//    void SetData(const void* parSrc, u32 parSize)
//    {
//        FSize = parSize / sizeof(u32);
//        FData = new u32[FSize];
//        AssertRelease(FData != nullptr);
//        memcpy(FData, parSrc, parSize);
//
//        AlwaysCheckedAssert(!FBool);
//        FHandle.idx = bgfx::createIndexBuffer(bgfx::makeRef(FData, FSize * sizeof(u32))).idx;
//        AlwaysCheckedAssert(bgfx::isValid(FHandle));
//        FBool = true;
//    }
//
// private:
//    u32* FData;
//    u32 FSize;
//
//    bool FBool;
//    bgfx::IndexBufferHandle FHandle;
//};

class C
{
};
class A
{
public:
    A() {}
    virtual ~A() { test = true; }

    bool test = false;
    bool test2 = false;
};
class B : public A
{
public:
    B()
        : A()
        , ptr(nullptr)
        , test2(false)
    {
    }
    virtual ~B() { test2 = true; }

    constexpr u32 getvalue() const { return 0; }

    C* ptr;
    bool test2;
};

int main(int argc, char** argv)
{
    // module params
    ECSEngine::ModuleParameters::InitParameterIdentifiersTraits();

    // init global resource cache
    ECSEngine::GlobalResourceCache::CreateIFP();
    ECSEngine::GlobalResourceCache::Instance().FCache = new ECSEngine::ResourceCache(
          10, new ECSEngine::ResourceFileDirectoryView("D:\\Programmation\\GameEngine\\ECSEngine\\Assets"));

    if (!ECSEngine::GlobalResourceCache::Instance().FCache->Initialize())
        AssertNotReachedMsg("Unable to init the resource cache!!");

    // init ecs
    ECSEngine::WorldManager::CreateIFP();
    AssertRelease(ECSEngine::WorldManager::HasInstance());
    ECSEngine::WorldManager& worldManagerInstance = ECSEngine::WorldManager::Instance();
    worldManagerInstance.Init();

    ECSEngine::EntityTemplateManager::CreateIFP();
    AssertRelease(ECSEngine::EntityTemplateManager::HasInstance());
    ECSEngine::EntityTemplate* newTemplate = ECSEngine::EntityTemplateManager::Instance().CreateNewEntityTemplate();
    newTemplate->SetHasModule<ECSEngine::ApparenceModule>();
    newTemplate->SetHasModule<ECSEngine::PositionModule>();

    // init rendering
    ECSEngine::Rendering::DisplayWindow::CreateIFP();
    ECSEngine::Rendering::DisplayWindow::Instance().Init();

    ECSEngine::Rendering::BGFXRenderer::CreateIFP();
    ECSEngine::Rendering::BGFXRenderer& rendererInstance = ECSEngine::Rendering::BGFXRenderer::Instance();
    rendererInstance.Init();

    ECSEngine::Rendering::ImGUI::Init();

    ECSEngine::RenderingSystem renderSystem;
    renderSystem.Init();

    ECSEngine::Rendering::MeshManager::CreateIFP();
    ECSEngine::Rendering::MeshHandle handle = ECSEngine::Rendering::MeshManager::Instance().CreateMesh(cubeVertices, sizeof(cubeVertices), cubeTriList, sizeof(cubeTriList));

    // init units
    std::vector<ECSEngine::EntityId> entities;
    ECSEngine::EntityWorld& world = worldManagerInstance.GetWorld(ECSEngine::Worlds::STANDARD);
    for (int i = -5; i < 6; ++i)
    {
        for (int j = -5; j < 6; ++j)
        {
            ECSEngine::ModuleParameters::ParameterContainer container;
            container.Set<ECSEngine::ModuleParameters::Mesh>(handle);
            container.Set<ECSEngine::ModuleParameters::Position>(glm::vec3((float)i, (float)j, 0.f));

            ECSEngine::EntityId unitId = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
            AssertRelease(unitId.Valid());
            entities.push_back(unitId);
        }
    }

    // main loop

    while (!ECSEngine::Rendering::DisplayWindow::Instance().ShouldClose())
    {
        ECSEngine::Rendering::ImGUI::NewFrame();

        // Updates
        ImGui::ShowDemoWindow();

        // Rendering
        renderSystem.Update();

        ECSEngine::Rendering::ImGUI::Render();

        ECSEngine::Rendering::BGFXRenderer::Instance().RenderFrame();

        ECSEngine::Rendering::DisplayWindow::Instance().PollEvents();
    }

    // destroy units
    foreachitem(id, entities) { world.DestroyEntity(id); }

    // shutdown rendering
    ECSEngine::Rendering::MeshManager::Destroy();

    renderSystem.Destroy();

    ECSEngine::Rendering::ImGUI::Shutdown();

    rendererInstance.Shutdown();
    ECSEngine::Rendering::BGFXRenderer::Destroy();

    ECSEngine::Rendering::DisplayWindow::Instance().Shutdown();
    ECSEngine::Rendering::DisplayWindow::Destroy();

    // destroy ecs
    ECSEngine::EntityTemplateManager::Destroy();
    worldManagerInstance.Destroy();

    // destroy resource cache
    ECSEngine::GlobalResourceCache::Destroy();

    // destroy module params
    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();
}
//    ECSEngine::ModuleParameters::InitParameterIdentifiersTraits();
//    // TestFunction();
//
//    ECSEngine::GlobalResourceCache::CreateIFP();
//    ECSEngine::GlobalResourceCache::Instance().FCache = new ECSEngine::ResourceCache(
//          10, new ECSEngine::ResourceFileDirectoryView("D:\\Programmation\\GameEngine\\ECSEngine\\Assets"));
//
//    if (!ECSEngine::GlobalResourceCache::Instance().FCache->Initialize())
//        AssertNotReachedMsg("Unable to init the resource cache!!");
//
//
//
//    ECSEngine::WorldManager::CreateIFP();
//    AssertRelease(ECSEngine::WorldManager::HasInstance());
//    ECSEngine::WorldManager& worldManagerInstance = ECSEngine::WorldManager::Instance();
//    worldManagerInstance.Init();
//
//    ECSEngine::EntityTemplateManager::CreateIFP();
//    AssertRelease(ECSEngine::EntityTemplateManager::HasInstance());
//    ECSEngine::EntityTemplate* newTemplate = ECSEngine::EntityTemplateManager::Instance().CreateNewEntityTemplate();
//    newTemplate->SetHasModule<ECSEngine::ApparenceModule>();
//
//    Data* d = new Data();
//    d->SetData(cubeTriList, sizeof(cubeTriList));
//    delete d;
//
//    using VertexLayout = ECSEngine::Rendering::VertexPositionColorN<1>;
//
//    ECSEngine::Rendering::IMesh* mesh = new ECSEngine::Rendering::Mesh<VertexLayout>();
//    mesh->SetRawVertexData(cubeVertices, sizeof(cubeVertices), true);
//    mesh->SetRawIndexData(cubeTriList, sizeof(cubeTriList), true);
//
//    bgfx::ProgramHandle program = ECSEngine::Rendering::LoadProgram("D:\\Programmation\\GameEngine\\ECSEngine\\Assets\\shaders\\Perso\\", "VertexColor");
//
//    bgfx::UniformHandle u_color = bgfx::createUniform("u_color", bgfx::UniformType::Vec4);
//
//    ECSEngine::ModuleParameters::ParameterContainer container;
//    container.Set<ECSEngine::ModuleParameters::Mesh>(mesh);
//    container.Set<ECSEngine::ModuleParameters::Material>(program);
//
//    ECSEngine::EntityWorld& world = worldManagerInstance.GetWorld(ECSEngine::Worlds::STANDARD);
//    ECSEngine::EntityId unitID = world.CreateEntityFromTemplateReturnEntityId(newTemplate, container);
//
//    ECSEngine::RenderingSystem renderingSystem;
//    renderingSystem.Init();
//
//    auto start = std::chrono::high_resolution_clock::now();
//    auto end = start;
//    auto timeElapsed = end - start;
//    unsigned int counter = 0;
//    while (!glfwWindowShouldClose(ECSEngine::Rendering::DisplayWindow::Instance().GetWindowHandle()))
//    {
//        auto this_tick = std::chrono::high_resolution_clock::now();
//        const float timepoint = (float)(this_tick - start).count() / 1000000000.0f;
//        const glm::uvec2 windowSize = ECSEngine::Rendering::DisplayWindow::Instance().GetSize();
//
//        const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
//        const bx::Vec3 eye = { 0.0f, 0.0f, -5.0f };
//        float view[16];
//        bx::mtxLookAt(view, eye, at);
//        float proj[16];
//        bx::mtxProj(proj, 60.0f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
//        bgfx::setViewTransform(0, view, proj);
//
//
//
//        rendererInstance.RenderFrame();
//        glfwPollEvents();
//        counter++;
//        end = std::chrono::high_resolution_clock::now();
//        auto timeElapsed = end - start;
//    }
//
//    world.DestroyEntity(unitID);
//
//    delete mesh;
//
//    ECSEngine::EntityTemplateManager::Destroy();
//
//    worldManagerInstance.Destroy();
//
//    rendererInstance.Shutdown();
//
//    ECSEngine::Rendering::DisplayWindow::Instance().Shutdown();
//

//
//    ECSEngine::ModuleParameters::DestroyParameterIdentifiersTraits();
//
//    return 0;
//}