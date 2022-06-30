#include "stdafx.h"

#include "GameScenarioUpdater.h"

#include "Common/CameraManager.h"
#include "Common/ResourceHandle.h"
#include "Common/SavingSystemImplementation.h"
#include "Common/TimeManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/WorldManager.h"
#include "ECSGameplay_Specific/CircularBuildingGrid.h"
#include "ECSGameplay_Specific/EnergySystem.h"
#include "ECSGameplay_Specific/MousePolicyManager.h"
#include "ECSGameplay_Specific/ResourceManager.h"
#include "ImGuiTools/ResourceCacheDebug.h"
#include "NavMeshPathfindingManager.h"
#include "Rendering/FinalCombinePass.h"
#include "Rendering/GameRenderer.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GFXRepresentationManager.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/RenderPass.h"
#include "SaveLoadManager.h"
#include "SelectionManager.h"
#include "UICore/RMLUIManager.h"

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

IMPLEMENT_SAVELOAD_ABILITIES(GameScenarioUpdater);
template<typename Chunk, bool isWriting>
void GameScenarioUpdater::SaveLoad(Chunk& parChunk)
{
    parChunk& FScenarioFileName;

    if (!isWriting)
    {
        FScenario->Destroy();
        delete FScenario;
        FScenario = nullptr;

        SetScenario(FScenarioFileName);
        AssertRelease(FScenario != nullptr);

        SelectionManager::Instance().ClearAll();
    }

    parChunk&(*FScenario);

    parChunk& WorldManager::Instance();

    parChunk& EnergySystem::Instance();

    // save resource manager ?
}

void GameScenarioUpdater::Initialise()
{
    Rendering::GameRenderer::CreateIFP();
    Rendering::GameRenderer::Instance().Initialise();

    UI::RmlUiManager::CreateIFP();
    UI::RmlUiManager::Instance().Initialise();

    Rendering::FinalCombinePass::CreateIFP();
    Rendering::FinalCombinePass::Instance().Initialise();

    MousePolicyManager::CreateIFP();
    MousePolicyManager::Instance().Initialise();

    SelectionManager::CreateIFP();
    SelectionManager::Instance().Init();

    AssertRelease(FScenario != nullptr);
    EnergySystem::CreateIFP();
    EnergySystem::Instance().Init();
    ResourceManager::CreateIFP();
    ResourceManager::Instance().Init();
    FCameraMoverSystem.Init();
    FRenderingSystem.Init();
    FRawResourceProductionSystem.Init();
    FResourceStatsUpdateSystem.Init();
    FStorageSlotSystem.Init();
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
    FStorageSlotSystem.Destroy();
    FResourceStatsUpdateSystem.Destroy();
    FRawResourceProductionSystem.Destroy();
    FRenderingSystem.Destroy();
    FCameraMoverSystem.Destroy();
    ResourceManager::Instance().Finalize();
    ResourceManager::Delete();
    EnergySystem::Instance().Finalize();
    EnergySystem::Delete();

    SelectionManager::Instance().Finalize();
    SelectionManager::Delete();

    MousePolicyManager::Instance().Shutdown();
    MousePolicyManager::Destroy();

    Rendering::FinalCombinePass::Instance().Shutdown();
    Rendering::FinalCombinePass::Destroy();

    UI::RmlUiManager::Instance().Shutdown();
    UI::RmlUiManager::Destroy();

    Rendering::GameRenderer::Instance().Shutdown();
    Rendering::GameRenderer::Destroy();
}

void GameScenarioUpdater::RealtimeUpdate()
{
    SCOPED_PROFILE(GameScenarioUpdater_RealtimeUpdate);
    FCameraMoverSystem.Update();

    if (Input::GetButtonDown(InputKeyNames::INPUT_KEY_F5))
    {
        SaveLoadManager::Instance().RequestQuickSave();
    }
    else if (Input::GetButtonDown(InputKeyNames::INPUT_KEY_F8))
    {
        SaveLoadManager::Instance().RequestQuickLoad();
    }
}

void GameScenarioUpdater::GameplayUpdate()
{
    {
        SCOPED_PROFILE(GameScenarioUpdater_GameplayUpdate);

        SelectionManager::Instance().Update();

        AssertRelease(FScenario != nullptr);
        FScenario->Update();

        EnergySystem::Instance().Update();
        FRawResourceProductionSystem.Update();
        FResourceStatsUpdateSystem.Update();
        FStorageSlotSystem.Update();

        FColonyBuildingSystem.Update();

        ResourceManager::Instance().Update();

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

    {
        SCOPED_PROFILE(GameScenarioUpdater_RmlUIUpdate);
        UI::RmlUiManager::Instance().Update();
    }
}

void GameScenarioUpdater::DebugRender()
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

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showDebugForCacheDebug, false, "show debug", "ResourceCache");
        if (showDebugForCacheDebug)
        {
            bool dummy = showDebugForCacheDebug;
            ImGUITools::DrawResourceCacheDebug(&dummy);
        }

        ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(showBGFXDebugs, false, "show debug", "Rendering");
        if (showBGFXDebugs)
        {
            bool dummy = showBGFXDebugs;
            Rendering::BGFXRenderingBackend::Instance().DrawStats(&dummy);
        }

        Pathfinding::Debug(*buffer, handle);

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
        SCOPED_PROFILE(GameScenarioUpdater_Feedback);
        FColonyFeedbackSystem.Update();
        FStorageFeedback.DrawFeedback();
    }

    {
        SCOPED_PROFILE(GameScenarioUpdater_RenderingFrame);

        Rendering::GFXRepresentationManager::Instance().OnGameplayFrameEnded();

        Rendering::GFXRepresentationManager::Instance().Update(TimeManager::FrameStartTime());

        Rendering::GameRenderer::Instance().Render();
    }

    {
        SCOPED_PROFILE(GameScenarioUpdater_Render_RmlUiManager);
        UI::RmlUiManager::Instance().Render();
    }

    {
        SCOPED_PROFILE(GameScenarioUpdater_Render_FinalCombinePass);
        Rendering::FinalCombinePass::Instance().SetTextures(Rendering::GameRenderer::Instance().GetFinalTexture(), UI::RmlUiManager::Instance().GetTexture());
        Rendering::FinalCombinePass::Instance().Render();
    }
}

void GameScenarioUpdater::EndUpdate()
{
    SCOPED_PROFILE_SIMPLE;

    SaveLoadManager::Instance().HandleSaveLoad(this);
}

void GameScenarioUpdater::SetScenario(const std::string& parScenarioFile)
{
    FScenarioFileName = parScenarioFile;
    AssertRelease(FScenario == nullptr);
    Resource r(parScenarioFile);
    auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
    ResourceBuffer buff = handle->GetResourceBuffer();
    std::istream istr(&buff, std::istream::in);
    cereal::JSONInputArchive ar(istr);

    FScenario = new SceneScenario();
    ar(*FScenario);
}

} // namespace ECSEngine
