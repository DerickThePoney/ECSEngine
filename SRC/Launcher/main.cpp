#include "stdafx.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceFileDirectoryView.h"
#include "Common/TimeManager.h"
#include "Common/Timer.h"
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

int main(int argc, char** argv)
{
    ECSEngine::TimeManager::CreateIFP();
    ECSEngine::TimeManager::Instance().Start();

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
    ECSEngine::Timer t;
    t.Start();
    while (!ECSEngine::Rendering::DisplayWindow::Instance().ShouldClose())
    {
        ECSEngine::TimeManager::Instance().NewFrame();
        ECSEngine::Rendering::ImGUI::NewFrame();

        // Updates
        ImGui::ShowDemoWindow();

        // Rendering
        renderSystem.Update();

        ECSEngine::Rendering::ImGUI::Render();

        ECSEngine::Rendering::BGFXRenderer::Instance().RenderFrame();

        ECSEngine::Rendering::DisplayWindow::Instance().PollEvents();
    }
    t.Stop();

    std::cout << t.ElapsedTime() << "\t" << t.ElapsedTime<ECSEngine::ETimePeriod::MILLISECONDS>() << "\t" << t.ElapsedTime<ECSEngine::ETimePeriod::MICROSECONDS>() << "\t"
              << t.ElapsedTime<ECSEngine::ETimePeriod::NANOSECONDS>() << std::endl;

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

    ECSEngine::TimeManager::Instance().End();
    ECSEngine::TimeManager::Destroy();
}