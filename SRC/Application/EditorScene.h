#pragma once
#include "Scene.h"

namespace ECSEngine
{
struct WindowsToShow
{
    bool showSceneItemsList = true;
    bool showActionManager = true;
    bool showEntityTemplateEditor = false;
    bool showLogger = false;
};

struct IOScene
{
    bool newScene = false;
    bool openScene = false;
    bool saveScene = false;
};

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

private:
    Scene* FCurrentScene;
    WindowsToShow FWindows;
    IOScene FIOScene;
};
} // namespace ECSEngine