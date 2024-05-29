#pragma once
#include "EditorCamera.h"
#include "IScenarioUpdater.h"

namespace ECSEngine
{
struct WindowsToShow
{
    bool showSceneItemsList = true;
    bool showActionManager = true;
    bool showEntityTemplateEditor = false;
    bool showGFXRepresentationsEditor = false;
    bool showProgramsEditor = false;
    bool showMaterialsEditor = false;
    bool showLogger = false;
    bool showImGuiDemo = false;
    bool showInputDebug = false;
    bool showPickingDebug = false;
    bool showEditorCameraParameters = false;
    bool showBGFXStatistics = false;
    bool showUIStyleEditor = false;
    bool showGameplayRulesEditor = false;
    bool showResourceCache = false;
    bool showScenesManagerEditor = false;
    bool showPhysicsConfigurationEditor = false;
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
class EditorGridRenderer;
class GameScenarioUpdater;
class SceneScenario;

namespace Rendering
{
class FeedbackRenderer;
}

class ScenarioEditor final : public IScenarioUpdater
{
public:
    ScenarioEditor();
    virtual ~ScenarioEditor();

    void Initialise() override;
    void Destroy() override;

    void RealtimeUpdate() override;
    void GameplayUpdate() override;
    void UIUpdate() override;
    void DebugRender() override;
    void Render() override;
    void EndUpdate() override;

    const SceneScenario* GetEditedScenario() const { return FCurrentScenario; }
    SceneScenario* GetEditedScenario() { return FCurrentScenario; }

    WindowsToShow& GetWindowsToShow() { return FWindows; }

    void UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect);

private:
    void UpdateForSceneEditing();
    void UpdateForInEditorPlaying();

    void UpdateSceneEditorStatus();

    void RenderForSceneEditing();
    void RenderForEditorPlaying();

private:
    SceneScenario* FCurrentScenario;
    WindowsToShow FWindows;
    IOScene FIOScene;
    EditorCamera FEditorCamera;
    ScenarioEditorStatus::Type FState;

    GameScenarioUpdater* FInGameScenarioPlayer;

    EditorSceneRenderer* FEditorSceneRenderer;
    SceneObjectsPickingRenderer* FEditorSceneObjectPickingRenderer;
    EditorGridRenderer* FEditorGridRenderer;
};
} // namespace ECSEngine
