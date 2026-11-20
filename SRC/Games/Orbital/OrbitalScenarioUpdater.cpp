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
#include "PlanetarySOIManager.h"
#include "Rendering/FinalCombinePass.h"
#include "Rendering/GameRenderer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MaterialManager.h"
#include "TimeWarpManager.h"
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
    FKeplerWarpCaptureSystem.Init();
    FKeplerWarpAdvanceSystem.Init();

    PlanetarySOIManager::CreateIFP();

    PhysicsJointSpawner::CreateIFP();

    TimeWarpManager::CreateIFP();
    TimeWarpManager::Instance().Initialize();

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

    TimeWarpManager::Destroy();
    PlanetarySOIManager::Destroy();

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
    FKeplerWarpCaptureSystem.Destroy();
    FKeplerWarpAdvanceSystem.Destroy();

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
    SCOPED_PROFILE_SIMPLE;

    AssertRelease(FScenario != nullptr);

    TimeWarpManager::Instance().ClampToAltitude();

    const bool bIsWarping = TimeWarpManager::Instance().CurrentTimeWarpMultiplier() > 1;
    if (!bIsWarping)
        FFloatingOriginSystem.Update();

    PlanetarySOIManager::Instance().Update();
    FScenario->Update();

    if (bIsWarping)
    {
        SCOPED_PROFILE(OrbitalScenarioUpdater::GameplayUpdate::TimeWarping);
        FKeplerWarpCaptureSystem.Update();

        u32 WarpMultiplier = TimeWarpManager::Instance().CurrentTimeWarpMultiplier();
        double InitialRemainingDT = TimeManager::GameplayDeltaTime() * WarpMultiplier;
        double RemainingDT = InitialRemainingDT;
        while (RemainingDT > 0 && WarpMultiplier > 1)
        {
            double step = Min(RemainingDT, 1.0);
            FPlanetOrbitSystem.ExternalAdvanceBy(step);
            FKeplerWarpAdvanceSystem.ExternalAdvanceBy(step);
            if (PlanetarySOIManager::Instance().Update())
            {
                // parent changed, recapture
                FKeplerWarpCaptureSystem.Update();
            }
            TimeWarpManager::Instance().ClampToAltitude();
            u32 NewWarpMultiplier = TimeWarpManager::Instance().CurrentTimeWarpMultiplier();

            if (NewWarpMultiplier == 1)
                break;

            RemainingDT -= step;

            if (NewWarpMultiplier != WarpMultiplier)
            {
                WarpMultiplier = NewWarpMultiplier;
                double NewInitialRemainingDT = TimeManager::GameplayDeltaTime() * WarpMultiplier;
                RemainingDT = RemainingDT * NewInitialRemainingDT / InitialRemainingDT;
                InitialRemainingDT = NewInitialRemainingDT;
            }
        }

        FFloatingOriginSystem.Update();
        FPhysicsSystem.ExternalSyncTransformsFromPhysics();
    }
    else
    {
        SCOPED_PROFILE(OrbitalScenarioUpdater::GameplayUpdate::PhysicsAdvance);
        FKeplerWarpCaptureSystem.Update(); // exit edge clears FbActive
        FRocketMassSystem.Update();
        FRocketSteeringSystem.Update();
        FRocketThrustSystem.Update();
        FRocketSASSystem.Update();
        FPlanetOrbitSystem.Update();
        FPlanetGravitySystem.Update();
        FPhysicsSystem.Update();
    }

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
        FPlanetOrbitSystem.DrawDebug(buffer, handle);
        FCameraMoverSystem.DrawDebug(buffer, handle);
        PlanetarySOIManager::Instance().DrawDebug(buffer, handle);

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
