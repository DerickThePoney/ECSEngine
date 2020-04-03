#include "stdafx.h"

#include "SceneEditor.h"

namespace ECSEngine
{
namespace ImGUITools
{
namespace
{
void MainMenuBar()
{
    ImGui::BeginMainMenuBar();
    ImGui::EndMainMenuBar();
}
} // namespace
void DrawSceneEditor()
{
    MainMenuBar();
}

} // namespace ImGUITools
} // namespace ECSEngine