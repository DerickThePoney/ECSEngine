#pragma once
#include "Application/Scene.h"
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

class EditorScene final : public Scene
{
public:
    EditorScene();
    ~EditorScene();

    void Initialise() override;
    void Destroy() override;

    void Update() override;
    void Render() override;

    const Scene* GetEditedScene() const { return FCurrentScene; }
    Scene* GetEditedScene() { return FCurrentScene; }

    WindowsToShow& GetWindowsToShow() { return FWindows; }

    void UpdateSelectedItems(const std::pair<u32, u32>& parSelectedItem, const bool parSelected, const bool parUnselect);

private:
    Scene* FCurrentScene;
    WindowsToShow FWindows;
    IOScene FIOScene;
    EditorCamera FEditorCamera;

    EditorSceneRenderer* FEditorSceneRenderer;
    SceneObjectsPickingRenderer* FEditorSceneObjectPickingRenderer;
};
} // namespace ECSEngine