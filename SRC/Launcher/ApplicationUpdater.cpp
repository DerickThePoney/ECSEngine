#include "stdafx.h"

#include "ApplicationUpdater.h"

#include "Application/SceneManager.h"
#include "Common/MainOptions.h"
#include "Common/TimeManager.h"
#include "ECSGameplay_Common/GameScenarioUpdater.h"
#include "ECSGameplay_Common/ScenarioEditor.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"

namespace ECSEngine
{
class ApplicationUpdater
{
public:
    void Initialise();

    void Shutdown();

    bool CheckShouldFinish();

    void StartUpdate();

    void UpdateGameplay();

    void UIUpdate();

    void DebugRender();

    void Render();

    void EndUpdate();

private:
    IScenarioUpdater* FScene;
};

void ApplicationUpdater::Initialise()
{
#ifndef COMPILE_FINAL
    if (Options.IsUsingEditor)
    {
        FScene = new ScenarioEditor();
    }
    else
    {
        const std::string startScene = SceneManager::Instance().GetSceneFilenameFromIndex(0);
        AssertRelease(!startScene.empty());
        GameScenarioUpdater* scene = new GameScenarioUpdater(); // TODO Main menu updater (//single ui system ?)
        scene->SetScenario(startScene);
        FScene = scene;
    }
#else
    const std::string startScene = SceneManager::Instance().GetSceneFilenameFromIndex(0);
    AssertRelease(!startScene.empty());
    GameScenarioUpdater* scene = new GameScenarioUpdater();
    scene->SetScenario(startScene);
    FScene = scene;
#endif

    AssertRelease(FScene != nullptr);
    FScene->Initialise();
}

void ApplicationUpdater::Shutdown()
{
    FScene->Destroy();

    delete FScene;
}

bool ApplicationUpdater::CheckShouldFinish()
{
    return Rendering::GLFWDisplayWindowHandler::Instance().ShouldClose();
}

void ApplicationUpdater::StartUpdate()
{
    TimeManager::NewFrame();

    Rendering::ImGUI::NewFrame();

    FScene->RealtimeUpdate();
}

void ApplicationUpdater::UpdateGameplay()
{
    while (TimeManager::FrameStartTime() >= TimeManager::CurrentGameplayTime())
    {
        SCOPED_PROFILE(ApplicationUpdater_OneGameplayTick);
        TimeManager::NewGameplayTick();
        FScene->GameplayUpdate();
    }
}

void ApplicationUpdater::UIUpdate()
{
    FScene->UIUpdate();
}

void ApplicationUpdater::DebugRender()
{
    FScene->DebugRender();
}

void ApplicationUpdater::Render()
{
    FScene->Render();

    {
        SCOPED_PROFILE(ApplicationUpdater_Render_Imgui);
        Rendering::ImGUI::Render();
    }

    {
        SCOPED_PROFILE(ApplicationUpdater_Render_Present);
        Rendering::BGFXRenderingBackend::Instance().RenderFrame();
    }
}

void ApplicationUpdater::EndUpdate()
{
    FScene->EndUpdate();
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
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, StartUpdate);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->StartUpdate();
}

void ApplicationUpdaterWrapper::GameplayUpdate()
{
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, GameplayUpdate);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->UpdateGameplay();
}

void ApplicationUpdaterWrapper::UIUpdate()
{
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, UIUpdate);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->UIUpdate();
}

void ApplicationUpdaterWrapper::DebugRender()
{
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, DebugRender);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->DebugRender();
}

void ApplicationUpdaterWrapper::Render()
{
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, Render);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Render();
}

void ApplicationUpdaterWrapper::EndUpdate()
{
    SCOPED_PROFILE_CLASS(ApplicationUpdaterWrapper, EndUpdate);
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->EndUpdate();
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::ApplicationUpdaterWrapper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::IGameplayUpdater, ECSEngine::ApplicationUpdaterWrapper);
