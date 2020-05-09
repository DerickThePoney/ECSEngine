#pragma once
#include "Application/SceneScenario.h"
#include "EditorCamera.h"
#include "IScenarioUpdater.h"

namespace ECSEngine
{
struct WindowsToShow
{
    bool showSceneItemsList = true;
    bool showActionManager = true;
    bool showEntityTemplateEditor = false;
    bool showLogger = false;
    bool showImGuiDemo = false;
    bool showInputDebug = false;
    bool showPickingDebug = false;
};

struct IOScene
{
    bool newScene = false;
    bool openScene = false;
    bool saveScene = false;
};

namespace ScenarioEditorStatus
{
enum Type
{
    EDITING_SCENARIO,
    PLAYING_SCENARIO,
    LENGTH
};
}

class SceneObjectsPickingRenderer;
class EditorSceneRenderer;

class ScenarioEditor final : public IScenarioUpdater
{
public:
    ScenarioEditor();
    virtual ~ScenarioEditor();

    void Initialise() override;
    void Destroy() override;

    void Update() override;
    void Render() override;

    const SceneScenario* GetEditedScenario() const { return FCurrentScenario; }
    SceneScenario* GetEditedScenario() { return FCurrentScenario; }

    WindowsToShow& GetWindowsToShow() { return FWindows; }

    void UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect);

private:
    SceneScenario* FCurrentScenario;
    WindowsToShow FWindows;
    IOScene FIOScene;
    EditorCamera FEditorCamera;
    ScenarioEditorStatus::Type FState;

    EditorSceneRenderer* FEditorSceneRenderer;
    SceneObjectsPickingRenderer* FEditorSceneObjectPickingRenderer;
};
} // namespace ECSEngine