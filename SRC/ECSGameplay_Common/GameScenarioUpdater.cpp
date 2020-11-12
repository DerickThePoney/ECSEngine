#include "stdafx.h"

#include "GameScenarioUpdater.h"

#include "Application/SceneScenario.h"
#include "Common/CameraManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "ECSCore/WorldManager.h"
#include "PathfindingManager.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DrawCommands.h"
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
    AssertRelease(FScenario != nullptr);
    FCameraMoverSystem.Init();
    FMovementSystem.Init();
    FRenderingSystem.Init();

    FScenario->Initialise();
}

void GameScenarioUpdater::Destroy()
{
    WorldManager::Instance().DestroyAllRemainingEntities();

    FScenario->Destroy();
    delete FScenario;
    FScenario = nullptr;

    FRenderingSystem.Destroy();
    FMovementSystem.Destroy();
    FCameraMoverSystem.Destroy();
}

void GameScenarioUpdater::Update()
{
    AssertRelease(FScenario != nullptr);
    FScenario->Update();
    FCameraMoverSystem.Update();
    FMovementSystem.Update();

    Rendering::ImGUI::SetImGuiContext(Rendering::RenderPassId::IMGUI_DEBUG_PASS);
    DrawAdjustables();
}

void GameScenarioUpdater::Render()
{
    AssertRelease(FScenario != nullptr);
    // FScenario->Render();

    FRenderingSystem.Update();

    Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
    u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
    Camera* camera = CameraManager::Instance().GetCamera(camId);
    buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
    Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");
    Pathfinding::Debug(*buffer, handle);
    FMovementSystem.VisualDebug(*buffer, handle);
    buffer->Submit();
    buffer->clear();
    delete buffer;
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