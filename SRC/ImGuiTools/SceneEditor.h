#pragma once
namespace ECSEngine
{
class Scene;
struct WindowsToShow;
struct IOScene;
namespace ImGUITools
{
void DrawSceneEditorMainMenu(Scene* parScene, WindowsToShow& parOutWindowsToShow, IOScene& parOutIOScene);
const std::string ChooseScene(bool& isDone, bool& isCancel);
const std::string NewScene(bool& isOk, bool& isCancel);
} // namespace ImGUITools
} // namespace ECSEngine
