#pragma once
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
class Scene;
class EditorSceneRenderer
{
public:
    EditorSceneRenderer();
    ~EditorSceneRenderer();

    void Initialise(const std::string& parHandleFileName, const std::string& parHandleMaterial);
    void Shutdown();

    void RenderScene(const Scene* parScene);

private:
    Rendering::MeshHandle FHandleMesh;
    Rendering::MaterialInstanceHandle FHandleMaterial;
};
} // namespace ECSEngine