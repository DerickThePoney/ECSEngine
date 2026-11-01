#include "stdafx.h"

#include "OrbitalScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "Common/CameraManager.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "Physics/PhysicsAPI.h"
#include "Rendering/FinalCombinePass.h"
#include "Rendering/GameRenderer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "UICore/RMLUIManager.h"

namespace ECSEngine
{
OrbitalScenarioUpdater::OrbitalScenarioUpdater() = default;

OrbitalScenarioUpdater::~OrbitalScenarioUpdater()
{
    delete FScenario;
}

void OrbitalScenarioUpdater::SetScenario(const std::string& parScenarioFile, bool bForceReload)
{
    FScenarioFileName = parScenarioFile;
    AssertRelease(FScenario == nullptr);
    Resource r(parScenarioFile);
    auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    if (bForceReload)
    {
        GlobalResourceCache::Instance().FCache->ForceReleaseResource(handle);
        handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    }

    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream istr(&buff, std::istream::in);
    cereal::JSONInputArchive ar(istr);

    FScenario = new SceneScenario();
    ar(*FScenario);
}

void OrbitalScenarioUpdater::Initialise()
{
    Rendering::GameRenderer::CreateIFP();
    Rendering::GameRenderer::Instance().Initialise();

    UI::RmlUiManager::CreateIFP();
    UI::RmlUiManager::Instance().Initialise();

    Rendering::FinalCombinePass::CreateIFP();
    Rendering::FinalCombinePass::Instance().Initialise();

    AssertRelease(FScenario != nullptr);
    FCameraMoverSystem.Init();
    FRenderingSystem.Init();
    FPhysicsSystem.Init();
    FRocketSystem.Init();
    PhysicsJointSpawner::CreateIFP();

    FScenario->Initialise();
}

void OrbitalScenarioUpdater::Destroy()
{
    WorldManager::Instance().DestroyAllRemainingEntities();

    if (FScenario != nullptr)
    {
        FScenario->Destroy();
        delete FScenario;
        FScenario = nullptr;
    }

    if (PhysicsJointSpawner::HasInstance())
    {
        PhysicsJointSpawner::Instance().Finalize();
        PhysicsJointSpawner::Delete();
    }
    FRocketSystem.Destroy();
    FPhysicsSystem.Destroy();
    FRenderingSystem.Destroy();
    FCameraMoverSystem.Destroy();

    Rendering::FinalCombinePass::Instance().Shutdown();
    Rendering::FinalCombinePass::Destroy();

    UI::RmlUiManager::Instance().Shutdown();
    UI::RmlUiManager::Destroy();

    Rendering::GameRenderer::Instance().Shutdown();
    Rendering::GameRenderer::Destroy();
}

void OrbitalScenarioUpdater::RealtimeUpdate()
{
    FCameraMoverSystem.Update();
}

void OrbitalScenarioUpdater::GameplayUpdate()
{
    AssertRelease(FScenario != nullptr);
    FScenario->Update();
    FRocketSystem.Update();
    FPhysicsSystem.Update();

    WorldManager::Instance().ProcessDestroyEntities();
    WorldManager::Instance().ProcessWithEntitiesCreation();

    if (PhysicsJointSpawner::HasInstance())
        PhysicsJointSpawner::Instance().Update();

    FRenderingSystem.Update();
}

void OrbitalScenarioUpdater::UIUpdate()
{
    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_UI_PASS);
    UI::RmlUiManager::Instance().Update();
}

void OrbitalScenarioUpdater::DebugRender()
{
#ifdef WITH_VISUAL_DEBUG
    Physics::DrawDebugs();
#endif
}

void OrbitalScenarioUpdater::Render()
{
    Rendering::GFXRepresentationManager::Instance().OnGameplayFrameEnded();
    Rendering::GFXRepresentationManager::Instance().Update(TimeManager::FrameStartTime());
    Rendering::GameRenderer::Instance().Render();

    UI::RmlUiManager::Instance().Render();
    Rendering::FinalCombinePass::Instance().SetTextures(Rendering::GameRenderer::Instance().GetFinalTexture(), UI::RmlUiManager::Instance().GetTexture());
    Rendering::FinalCombinePass::Instance().Render();
}

void OrbitalScenarioUpdater::EndUpdate()
{
}
} // namespace ECSEngine
