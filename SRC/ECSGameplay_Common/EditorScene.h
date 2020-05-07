#pragma once
#include "Application/SceneScenario.h"
#include "EditorCamera.h"

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

class SceneObjectsPickingRenderer;
class EditorSceneRenderer;

class EditorScene final
{
public:
    EditorScene();
    ~EditorScene();

    void Initialise();
    void Destroy();

    void Update();
    void Render();

    const SceneScenario* GetEditedScenario() const { return FCurrentScenario; }
    SceneScenario* GetEditedScenario() { return FCurrentScenario; }

    WindowsToShow& GetWindowsToShow() { return FWindows; }

    void UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect);

private:
    SceneScenario* FCurrentScenario;
    WindowsToShow FWindows;
    IOScene FIOScene;
    EditorCamera FEditorCamera;

    EditorSceneRenderer* FEditorSceneRenderer;
    SceneObjectsPickingRenderer* FEditorSceneObjectPickingRenderer;
};
} // namespace ECSEngine