#include "stdafx.h"

#include "OrbitalScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "Common/CameraManager.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "Common/TimeManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Common/PhysicsJointSpawner.h"
#include "Physics/PhysicsAPI.h"
#include "Rendering/FinalCombinePass.h"
#include "Rendering/GameRenderer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MaterialManager.h"
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
    FFloatingOriginSystem.Init();
    FCameraMoverSystem.Init();
    FRenderingSystem.Init();
    FPhysicsSystem.Init();
    FRocketControllerSystem.Init();
    FRocketSteeringSystem.Init();
    FRocketThrustSystem.Init();
    FRocketSASSystem.Init();
    FPlanetGravitySystem.Init();
    FRocketMassSystem.Init();
    FPlanetOrbitSystem.Init();

    PhysicsJointSpawner::CreateIFP();

    FRocketUIController.Init();

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

    FRocketUIController.Destroy();

    if (PhysicsJointSpawner::HasInstance())
    {
        PhysicsJointSpawner::Instance().Finalize();
        PhysicsJointSpawner::Delete();
    }
    FFloatingOriginSystem.Destroy();
    FRocketMassSystem.Destroy();
    FRocketSASSystem.Destroy();
    FRocketThrustSystem.Destroy();
    FRocketSteeringSystem.Destroy();
    FRocketControllerSystem.Destroy();
    FPhysicsSystem.Destroy();
    FRenderingSystem.Destroy();
    FCameraMoverSystem.Destroy();
    FPlanetGravitySystem.Destroy();
    FPlanetOrbitSystem.Destroy();

    Rendering::FinalCombinePass::Instance().Shutdown();
    Rendering::FinalCombinePass::Destroy();

    UI::RmlUiManager::Instance().Shutdown();
    UI::RmlUiManager::Destroy();

    Rendering::GameRenderer::Instance().Shutdown();
    Rendering::GameRenderer::Destroy();
}

void OrbitalScenarioUpdater::RealtimeUpdate()
{
    FRocketControllerSystem.Update();
}

void OrbitalScenarioUpdater::GameplayUpdate()
{
    AssertRelease(FScenario != nullptr);
    FFloatingOriginSystem.Update();
    FScenario->Update();
    FRocketMassSystem.Update();
    FRocketSteeringSystem.Update();
    FRocketThrustSystem.Update();
    FRocketSASSystem.Update();
    FPlanetOrbitSystem.Update();
    FPlanetGravitySystem.Update();
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
    FRocketUIController.Update();
    UI::RmlUiManager::Instance().Update();
}

void OrbitalScenarioUpdater::DebugRender()
{
#ifdef WITH_VISUAL_DEBUG
    {
        SCOPED_PROFILE(GameScenarioUpdater_DebugUpdate);
        Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_DEBUG_PASS);
        DrawAdjustables();

        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showBGFXDebugs, false, "show debug", "Rendering");
        if (showBGFXDebugs)
        {
            bool dummy = showBGFXDebugs;
            Rendering::BGFXRenderingBackend::Instance().DrawStats(&dummy);
        }

        FRocketUIController.DrawDebug(buffer, handle);

        buffer->Submit();
        buffer->clear();
        delete buffer;

        Physics::DrawDebugs();
    }
#endif
}

void OrbitalScenarioUpdater::Render()
{
    Rendering::GFXRepresentationManager::Instance().OnGameplayFrameEnded();
    Rendering::GFXRepresentationManager::Instance().Update(TimeManager::FrameStartTime());

    FCameraMoverSystem.Update();
    Rendering::GameRenderer::Instance().Render();

    UI::RmlUiManager::Instance().Render();
    Rendering::FinalCombinePass::Instance().SetTextures(Rendering::GameRenderer::Instance().GetFinalTexture(), UI::RmlUiManager::Instance().GetTexture());
    Rendering::FinalCombinePass::Instance().Render();
}

void OrbitalScenarioUpdater::EndUpdate()
{
}
} // namespace ECSEngine
