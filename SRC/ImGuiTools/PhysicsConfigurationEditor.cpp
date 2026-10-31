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

    ImGui::SeparatorText("Gravity value");
    if (ImGui::InputFloat("Gravity value", &Config.FGravityValue))
    {
        bHasChanged = true;
    }

    ImGui::SeparatorText("Broad phase configuration");
    if (ImGui::InputFloat("AABB fattening value", &Config.FAABBFattenValue))
    {
        bHasChanged = true;
    }

    if (ImGui::InputFloat("AABB displacement multiplier", &Config.FAABBDisplacementMultiplier))
    {
        bHasChanged = true;
    }

    ImGui::SeparatorText("Island solver configuration");
    {
        int SolverIterations = (int)Config.FSolverIterations;
        if (ImGui::InputInt("Solver iterations", &SolverIterations))
        {
            Config.FSolverIterations = (u32)Max(1, SolverIterations);
            bHasChanged = true;
        }
    }

    ImGui::SeparatorText("Contact solver configuration");
    if (ImGui::InputFloat("Baumgarte", &Config.FBaumgarte))
    {
        bHasChanged = true;
    }

    if (ImGui::InputFloat("Penetration slop", &Config.FPenetrationSlop))
    {
        bHasChanged = true;
    }

    ImGui::SeparatorText("Contact solver configuration");
    if (ImGui::CollapsingHeader("Collision Layers"))
    {
        ImGui::Indent();
        forrange(i, 0, Config.FNumLayers)
        {
            ImGui::PushID(i);
            ImGui::Text(std::format("Layer {}", i).c_str());
            ImGui::SameLine();

            char buff[1024];
            sprintf(buff, "%s", Config.FLayers[i].c_str());
            if (ImGui::InputText("##LayerEdit", buff, 1024))
            {
                Config.FLayers[i] = std::string(buff);
                bHasChanged = true;
            }
            ImGui::PopID();
        }

        if (Config.FNumLayers < 32)
        {
            if (ImGui::Button("Add##CollisionLayer"))
            {
                Config.FNumLayers++;
                bHasChanged = true;
            }
        }

        ImGui::Unindent();
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