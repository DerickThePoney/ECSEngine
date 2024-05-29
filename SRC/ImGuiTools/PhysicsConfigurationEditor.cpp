#include "stdafx.h"

#include "PhysicsConfigurationEditor.h"

#include "Physics/PhysicsAPI.h"
#include "Physics/PhysicsEngineConfiguration.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"

#include <fstream>

namespace ECSEngine
{
namespace ImGUITools
{
class PhysicsConfigurationEditor
{
public:
    void DrawEditor(bool* parOpen, float parMenuBarHeight);

private:
    void DrawPhysicsConfigMenuBar();
};

void PhysicsConfigurationEditor::DrawEditor(bool* parOpen, float parMenuBarHeight)
{
    const uvec2 uwindowSize = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    const vec2 windowSize(uwindowSize.x, uwindowSize.y);
    ImGui::SetNextWindowSize(windowSize * vec2(1.0f, 1.0f - (parMenuBarHeight / windowSize.y)));
    ImGui::SetNextWindowPos(vec2(0.0f, parMenuBarHeight));
    ImGui::Begin("Physics configuration editor", parOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    DrawPhysicsConfigMenuBar();

    Physics::PhysicsEngineConfiguration Config = Physics::GetConfig();

    bool bHasChanged = false;
    if (ImGui::InputFloat("Gravity value", &Config.FGravityValue))
    {
        bHasChanged = true;
    }

    if (bHasChanged)
    {
        Physics::SetConfig(Config);
    }

    ImGui::End();
}

void PhysicsConfigurationEditor::DrawPhysicsConfigMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                std::ofstream ofstr(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\Configuration\\PhysicsConfiguration.json");

                cereal::JSONOutputArchive archive(ofstr);
                const Physics::PhysicsEngineConfiguration& Config = Physics::GetConfig();
                archive(NAMEDPROPERTY("PhysicsEngineConfiguration", Config));
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void DrawPhysicsConfigurationEditor(bool* parOpen, float parMenuBarHeight)
{
    static PhysicsConfigurationEditor editor;
    editor.DrawEditor(parOpen, parMenuBarHeight);
}

} // namespace ImGUITools
} // namespace ECSEngine