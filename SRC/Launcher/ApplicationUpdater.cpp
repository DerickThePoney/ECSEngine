#include "stdafx.h"

#include "ApplicationUpdater.h"

#include "Common/TimeManager.h"
#include "ECSGameplay_Common/ScenarioEditor.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"

namespace ECSEngine
{

namespace
{
// void AllocateUnits(const EntityTemplate* temp, EntityWorld& world, std::vector<EntityId>& entities)
//{
//    TScopedTimer st("Allocate 900 Units");
//    entities.reserve(entities.size() + 900);
//    for (int i = -50; i < 50; ++i)
//    {
//        for (int j = -4; j < 5; ++j)
//        {
//            ModuleParameters::ParameterContainer container;
//            container.Set<ModuleParameters::Position>(glm::vec3((float)i, (float)j, 0.f));
//
//            EntityId unitId = EntityFactory::CreateEntity(temp, container);
//            entities.push_back(unitId);
//        }
//    }
//}
//
// void StressTestDebug(const EntityTemplate* temp, RingBuffer<float, 100>& frameTimeBuffer, EntityWorld& world, std::vector<EntityId>& entities)
//{
//    ImGui::Begin("Stress test");
//    int realVal = (int)entities.size();
//    ImGui::InputInt("Current number of entities", &realVal, 1, 100, ImGuiInputTextFlags_ReadOnly);
//    float frameTime = TimeManager::FrameDeltaTime();
//    frameTimeBuffer.Push((frameTime == 0.0f) ? frameTime : 1.f / frameTime);
//    ImGui::InputFloat("Frame Time", &frameTime, 1, 100, "%.5f", ImGuiInputTextFlags_ReadOnly);
//    ImGui::PlotHistogram("FPS", frameTimeBuffer.data(), frameTimeBuffer.GetSize(), frameTimeBuffer.GetWriteHeadPosition(), "", 0.0f, 150.0f, ImVec2(0.0f, 45.0f));
//    /*if (ImGui::Button("Add 900 units"))
//    {
//        AllocateUnits(temp, world, entities);
//    }*/
//    ImGui::End();
//}

} // namespace

void ApplicationUpdater::Initialise()
{
    scene = new ScenarioEditor();
    scene->Initialise();
}

void ApplicationUpdater::Shutdown()
{
    scene->Destroy();

    delete scene;
}

bool ApplicationUpdater::CheckShouldFinish()
{
    return Rendering::GLFWDisplayWindowHandler::Instance().ShouldClose();
}

void ApplicationUpdater::StartUpdate()
{
    TimeManager::NewFrame();

    Rendering::ImGUI::NewFrame();
}

void ApplicationUpdater::Update()
{
    scene->Update();
}

void ApplicationUpdater::Render()
{
    scene->Render();

    Rendering::ImGUI::Render();

    Rendering::BGFXRenderer::Instance().RenderFrame();
}

void ApplicationUpdater::EndUpdate()
{
    Input::EndFrame();
    Rendering::GLFWDisplayWindowHandler::Instance().PollEvents();
}

ApplicationUpdaterWrapper::ApplicationUpdaterWrapper()
    : FWrappedGameplayUpdater(nullptr)
{
}

ApplicationUpdaterWrapper::~ApplicationUpdaterWrapper()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
}

void ApplicationUpdaterWrapper::Initialise()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
    FWrappedGameplayUpdater = new ApplicationUpdater();

    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Initialise();
}

void ApplicationUpdaterWrapper::Shutdown()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Shutdown();

    delete FWrappedGameplayUpdater;
    FWrappedGameplayUpdater = nullptr;
}

bool ApplicationUpdaterWrapper::CheckShouldFinish()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    return FWrappedGameplayUpdater->CheckShouldFinish();
}

void ApplicationUpdaterWrapper::StartUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->StartUpdate();
}

void ApplicationUpdaterWrapper::Update()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Update();
}

void ApplicationUpdaterWrapper::Render()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Render();
}

void ApplicationUpdaterWrapper::EndUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->EndUpdate();
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::ApplicationUpdaterWrapper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::IGameplayUpdater, ECSEngine::ApplicationUpdaterWrapper);