#pragma once
namespace ECSEngine
{
class SceneScenario;
struct WindowsToShow;
struct IOScene;
namespace ImGUITools
{
void DrawSceneEditorMainMenu(SceneScenario* parScene, WindowsToShow& parOutWindowsToShow, IOScene& parOutIOScene);
void DrawPlayScenarioWindow(bool& parOutPlayScenario);
const std::string ChooseScenario(bool& isDone, bool& isCancel);
const std::string NewScenario(bool& isOk, bool& isCancel);
} // namespace ImGUITools
} // namespace ECSEngine
