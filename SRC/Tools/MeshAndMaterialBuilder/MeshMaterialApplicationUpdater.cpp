#include "stdafx.h"

#include "MeshMaterialApplicationUpdater.h"

#include "Common/Logger.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/RingBuffer.h"
#include "Common/TimeManager.h"
#include "Common/Timer.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MeshManager.h"

#define WITH_ASSETS_GENERATION
#include "RenderingCore/MeshFileStreaming.h"

namespace ECSEngine
{
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

MeshMaterialApplicationUpdater::MeshMaterialApplicationUpdater()
    : FShouldClose(false)
{
}

void MeshMaterialApplicationUpdater::Initialise()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*", FMeshFiles);
}

void MeshMaterialApplicationUpdater::Shutdown()
{
}

bool MeshMaterialApplicationUpdater::CheckShouldFinish()
{
    return Rendering::GLFWDisplayWindowHandler::Instance().ShouldClose() || FShouldClose;
}

void MeshMaterialApplicationUpdater::StartUpdate()
{
    Rendering::ImGUI::NewFrame();
}

void MeshMaterialApplicationUpdater::Update()
{
    glm::uvec2 windowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(windowSize);
    ImGui::Begin("DataInfo", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

    ImGui::LabelText("", "Number of files found: %d", FMeshFiles.size());

    const glm::vec2 currentWindowSize = ImGui::GetWindowSize();
    const glm::vec2 loggerSize = currentWindowSize - 50.0f;
    ImGui::SetCursorPosX(((currentWindowSize - loggerSize) * 0.5f).x);

    ImGui::BeginChildFrame(ImGui::GetID("Test"), loggerSize);
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::LabelText("", "Test");
    ImGui::EndChildFrame();

    ImGui::End();
}

void MeshMaterialApplicationUpdater::Render()
{
    Rendering::ImGUI::Render();

    Rendering::BGFXRenderer::Instance().RenderFrame();
}

void MeshMaterialApplicationUpdater::EndUpdate()
{
    Rendering::GLFWDisplayWindowHandler::Instance().PollEvents();
}

MeshMaterialApplicationUpdaterWrapper::MeshMaterialApplicationUpdaterWrapper()
    : FWrappedGameplayUpdater(nullptr)
{
}

MeshMaterialApplicationUpdaterWrapper::~MeshMaterialApplicationUpdaterWrapper()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
}

void MeshMaterialApplicationUpdaterWrapper::Initialise()
{
    AssertRelease(FWrappedGameplayUpdater == nullptr);
    FWrappedGameplayUpdater = new MeshMaterialApplicationUpdater();

    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Initialise();
}

void MeshMaterialApplicationUpdaterWrapper::Shutdown()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Shutdown();

    delete FWrappedGameplayUpdater;
    FWrappedGameplayUpdater = nullptr;
}

bool MeshMaterialApplicationUpdaterWrapper::CheckShouldFinish()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    return FWrappedGameplayUpdater->CheckShouldFinish();
}

void MeshMaterialApplicationUpdaterWrapper::StartUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->StartUpdate();
}

void MeshMaterialApplicationUpdaterWrapper::Update()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Update();
}

void MeshMaterialApplicationUpdaterWrapper::Render()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->Render();
}

void MeshMaterialApplicationUpdaterWrapper::EndUpdate()
{
    AssertRelease(FWrappedGameplayUpdater != nullptr);
    FWrappedGameplayUpdater->EndUpdate();
}

} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::MeshMaterialApplicationUpdaterWrapper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::IGameplayUpdater, ECSEngine::MeshMaterialApplicationUpdaterWrapper);