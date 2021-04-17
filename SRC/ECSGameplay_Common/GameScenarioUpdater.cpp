#include "stdafx.h"

#include "GameScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "Common/CameraManager.h"
#include "Common/TimeManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Specific/CircularBuildingGrid.h"
#include "ECSGameplay_Specific/MousePolicyManager.h"
#include "ImGuiTools/ResourceCacheDebug.h"
#include "NavMeshPathfindingManager.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderPass.h"

namespace ECSEngine
{

GameScenarioUpdater::GameScenarioUpdater()
    : FScenario(nullptr)
{
}

GameScenarioUpdater::~GameScenarioUpdater()
{
    delete FScenario;
}

void GameScenarioUpdater::Initialise()
{
    Rendering::GameRenderer::CreateIFP();
    Rendering::GameRenderer::Instance().Initialise();

    MousePolicyManager::CreateIFP();
    MousePolicyManager::Instance().Initialise();

    AssertRelease(FScenario != nullptr);
    FCameraMoverSystem.Init();
    FMovementSystem.Init();
    FRenderingSystem.Init();
    FColonyManagementSystem.Init();
    FPeonHarvestingSytem.Init();
    FProductionSystem.Init();
    FPeonSpawnSystem.Init();
    FPeonLifeSpanSystem.Init();
    FResourceStatsUpdateSystem.Init();
    FHousingSystem.Init();
    FWorkSystem.Init();
    FColonyFeedbackSystem.Init();
    FColonyBuildingSystem.Init();
    FUserInterfaceSystem.Init();

    FScenario->Initialise();
}

void GameScenarioUpdater::Destroy()
{
    WorldManager::Instance().DestroyAllRemainingEntities();

    FScenario->Destroy();
    delete FScenario;
    FScenario = nullptr;

    FUserInterfaceSystem.Destroy();
    FColonyBuildingSystem.Destroy();
    FColonyFeedbackSystem.Destroy();
    FWorkSystem.Destroy();
    FHousingSystem.Destroy();
    FResourceStatsUpdateSystem.Destroy();
    FPeonLifeSpanSystem.Destroy();
    FPeonSpawnSystem.Destroy();
    FProductionSystem.Destroy();
    FPeonHarvestingSytem.Destroy();
    FColonyManagementSystem.Destroy();
    FRenderingSystem.Destroy();
    FMovementSystem.Destroy();
    FCameraMoverSystem.Destroy();

    WorldManager::Instance().DestroyAllRemainingEntities();

    MousePolicyManager::Instance().Shutdown();
    MousePolicyManager::Destroy();

    Rendering::GameRenderer::Instance().Shutdown();
    Rendering::GameRenderer::Destroy();
}

void GameScenarioUpdater::GameplayUpdate()
{
    {
        SCOPED_PROFILE(GameScenarioUpdater_GameplayUpdate);

        AssertRelease(FScenario != nullptr);
        FScenario->Update();
        FCameraMoverSystem.Update();
        FMovementSystem.Update();

        FColonyManagementSystem.Update();
        FPeonHarvestingSytem.Update();
        FProductionSystem.Update();
        FPeonSpawnSystem.Update();
        FPeonLifeSpanSystem.Update();
        FResourceStatsUpdateSystem.Update();
        FHousingSystem.Update();
        FWorkSystem.Update();

        FColonyBuildingSystem.Update();

        WorldManager::Instance().ProcessDestroyEntities();
    }

    {
        SCOPED_PROFILE(GameScenarioUpdater_SynchroWithRenderingUpdate);
        FRenderingSystem.Update();
    }
}

void GameScenarioUpdater::UIUpdate()
{
    {
        SCOPED_PROFILE(GameScenarioUpdater_UIUpdate_MousePolicies);
        MousePolicyManager::Instance().Update();
    }
    {
        SCOPED_PROFILE(GameScenarioUpdater_UIUpdate);
        Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_UI_PASS);
        FUserInterfaceSystem.Update();
    }
}

void GameScenarioUpdater::DebugRender()
{
#ifdef WITH_VISUAL_DEBUG
    {
        SCOPED_PROFILE(GameScenarioUpdater_DebugUpdate);
        Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_DEBUG_PASS);
        DrawAdjustables();

        FPeonHarvestingSytem.Debug();

        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showDebugForCacheDebug, false, "show debug", "ResourceCache");
        if (showDebugForCacheDebug)
        {
            bool dummy = showDebugForCacheDebug;
            ImGUITools::DrawResourceCacheDebug(&dummy);
        }

        Pathfinding::Debug(*buffer, handle);
        FMovementSystem.VisualDebug(*buffer, handle);

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showDebugForCircularGraph, false, "show debug", "Pathfinding/CircularGraph");
        if (showDebugForCircularGraph)
        {
            CircularBuildingGrid::Instance().GetGraph().Debug(*buffer, handle);
        }

        buffer->Submit();
        buffer->clear();
        delete buffer;
    }
#endif
}

void GameScenarioUpdater::Render()
{
    {
        SCOPED_PROFILE(GameScenarioUpdater_RenderingFrame);
        FColonyFeedbackSystem.Update();

        Rendering::GFXRepresentationManager::Instance().OnGameplayFrameEnded();

        Rendering::GFXRepresentationManager::Instance().Update(TimeManager::FrameStartTime());

        Rendering::GameRenderer::Instance().Render();
    }
}

void GameScenarioUpdater::SetScenario(const std::string& parScenarioFile)
{
    AssertRelease(FScenario == nullptr);
    std::ifstream ofstr(parScenarioFile);
    cereal::JSONInputArchive ar(ofstr);

    FScenario = new SceneScenario();
    ar(*FScenario);
}

} // namespace ECSEngine
