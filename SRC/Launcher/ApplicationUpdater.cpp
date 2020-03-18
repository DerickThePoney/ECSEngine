#include "stdafx.h"

#include "ApplicationUpdater.h"

#include "Common/InputManager.h"
#include "Common/MeshHandle.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "Common/RingBuffer.h"
#include "Common/TimeManager.h"
#include "Common/Timer.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/GLFWWrapper.h"
#include "RenderingCore/ImguiRenderer.h"
#include "RenderingCore/MeshManager.h"

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

namespace
{
void AllocateUnits(const EntityTemplate* temp, EntityWorld& world, Rendering::MeshHandle& handle, std::vector<EntityId>& entities)
{
    TScopedTimer st("Allocate 900 Units");
    entities.reserve(entities.size() + 900);
    for (int i = -50; i < 50; ++i)
    {
        for (int j = -4; j < 5; ++j)
        {
            ModuleParameters::ParameterContainer container;
            container.Set<ModuleParameters::Mesh>(handle);
            container.Set<ModuleParameters::Position>(glm::vec3((float)i, (float)j, 0.f));

            EntityId unitId = world.CreateEntityFromTemplateReturnEntityId(temp, container);
            AssertRelease(unitId.Valid());
            entities.push_back(unitId);
        }
    }
}

void StressTestDebug(const EntityTemplate* temp, RingBuffer<float, 100>& frameTimeBuffer, EntityWorld& world, Rendering::MeshHandle& handle, std::vector<EntityId>& entities)
{
    ImGui::Begin("Stress test");
    int realVal = (int)entities.size();
    ImGui::InputInt("Current number of entities", &realVal, 1, 100, ImGuiInputTextFlags_ReadOnly);
    float frameTime = TimeManager::FrameDeltaTime();
    frameTimeBuffer.Push((frameTime == 0.0f) ? frameTime : 1.f / frameTime);
    ImGui::InputFloat("Frame Time", &frameTime, 1, 100, "%.5f", ImGuiInputTextFlags_ReadOnly);
    ImGui::PlotHistogram("FPS", frameTimeBuffer.data(), frameTimeBuffer.GetSize(), frameTimeBuffer.GetWriteHeadPosition(), "", 0.0f, 150.0f, ImVec2(0.0f, 45.0f));
    if (ImGui::Button("Add 900 units"))
    {
        AllocateUnits(temp, world, handle, entities);
    }
    ImGui::End();
}

void InputDebug()
{
    ImGui::Begin("Input Debug");
    glm::vec2 mousePos = Input::GetMousePosition();
    glm::vec2 mouseScroll = Input::GetMouseScrollDelta();

    ImGui::InputFloat2("Mouse position", (float*)&mousePos, 3, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputFloat2("Mouse scroll", (float*)&mouseScroll, 3, ImGuiInputTextFlags_ReadOnly);

    forrange(i, 0, 7)
    {
        bool mouseButtonState = Input::GetMouseButtonState(i);
        ImGui::Checkbox("Mouse button", &mouseButtonState);
    }

    ImGui::Separator();

    bool keyboardButtonState = Input::IsShiftDown();
    ImGui::Checkbox("Shift", &keyboardButtonState);
    keyboardButtonState = Input::IsAltDown();
    ImGui::Checkbox("Alt", &keyboardButtonState);
    keyboardButtonState = Input::IsCtrlDown();
    ImGui::Checkbox("Ctrl", &keyboardButtonState);

    forrange(i, InputKeyNames::INPUT_KEY_COMMA, InputKeyNames::INPUT_KEY_RIGHT_BRACKET)
    {
        keyboardButtonState = Input::GetButtonDown((InputKeyNames::Type)i);
        ImGui::Checkbox(GLFWWrapper::GetKeyName((InputKeyNames::Type)i), &keyboardButtonState);

        if ((i - InputKeyNames::INPUT_KEY_A) % 5 != 0 || i == InputKeyNames::INPUT_KEY_A)
        {
            ImGui::SameLine();
        }
    }

    ImGui::Separator();
    forrange(i, 0, 16)
    {
        if (Input::IsGamepadConnected(i))
        {
            ImGui::Text(Input::GetGamepadName(i));

            forrange(j, 0, GamepadButtons::GAMEPAD_BUTTON_LAST)
            {
                bool isPressed = Input::GetGamepadButtonDown(i, (GamepadButtons::Type)j);
                ImGui::Checkbox(GamepadButtons::ToString((GamepadButtons::Type)j), &isPressed);
            }

            forrange(j, 0, GamepadAxes::GAMEPAD_AXIS_LAST)
            {
                float value = Input::GetGamepadAxisValue(i, (GamepadAxes::Type)j);
                ImGui::InputFloat(GamepadAxes::ToString((GamepadAxes::Type)j), &value, ImGuiInputTextFlags_ReadOnly);
            }
        }
    }
    ImGui::End();
}

void SaveAndReloadTest()
{
    ImGui::Begin("Save and Reload test");

    if (ImGui::Button("Save entities"))
    {
        std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\EntityTemplates.json");

        cereal::JSONOutputArchive archive(ofstr);

        archive(NAMEDPROPERTY("EntityTemplatesList", EntityTemplateManager::Instance()));
    }

    if (ImGui::Button("Load Entities"))
    {
        std::ifstream ifstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\EntityTemplates.json");

        cereal::JSONInputArchive archive(ifstr);

        EntityTemplateManager::Destroy();
        EntityTemplateManager::CreateIFP();

        archive(NAMEDPROPERTY("EntityTemplatesList", EntityTemplateManager::Instance()));
    }

    ImGui::End();
}
} // namespace

void ApplicationUpdater::Initialise()
{
    renderSystem.Init();
    orientationSystem.Init();

    std::vector<std::string> meshFiles;
    GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles("*.fbx.gen", meshFiles);

    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\" + meshFiles[1]);
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(0);

    AllocateUnits(FTemplate, WorldManager::Instance().GetWorld(Worlds::STANDARD), FMeshHandle, FEntities);
}

void ApplicationUpdater::Shutdown()
{
    foreachitem(id, FEntities) { WorldManager::Instance().GetWorld(Worlds::STANDARD).DestroyEntity(id); }

    orientationSystem.Destroy();
    renderSystem.Destroy();
}

bool ApplicationUpdater::CheckShouldFinish()
{
    return Rendering::GLFWDisplayWindowHandler::Instance().ShouldClose();
}

void ApplicationUpdater::StartUpdate()
{
    TimeManager::NewFrame();

    Input::NewFrame();

    Rendering::ImGUI::NewFrame();
}

void ApplicationUpdater::Update()
{
    // Updates
    ImGui::ShowDemoWindow();
    StressTestDebug(FTemplate, FFrameTimeBuffer, WorldManager::Instance().GetWorld(Worlds::STANDARD), FMeshHandle, FEntities);
    SaveAndReloadTest();
    InputDebug();

    orientationSystem.Update();
}

void ApplicationUpdater::Render()
{
    renderSystem.Update();

    Rendering::ImGUI::Render();

    Rendering::BGFXRenderer::Instance().RenderFrame();
}

void ApplicationUpdater::EndUpdate()
{
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