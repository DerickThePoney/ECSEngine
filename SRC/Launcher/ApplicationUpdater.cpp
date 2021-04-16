#include "stdafx.h"

#include "ApplicationUpdater.h"

#include "Common/TimeManager.h"
#include "ECSGameplay_Common/ScenarioEditor.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"

namespace ECSEngine
{

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

void ApplicationUpdater::UpdateGameplay()
{
    scene->GameplayUpdate();
}

void ApplicationUpdater::UIUpdate()
{
    scene->UIUpdate();
}

void ApplicationUpdater::DebugRender()
{
    scene->DebugRender();
}

void ApplicationUpdater::Render()
{
    scene->Render();

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
