#include "stdafx.h"

#include "EditorSceneRenderer.h"

#include "Application/Scene.h"
#include "RenderingCore/BGFXRenderer.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

namespace ECSEngine
{

EditorSceneRenderer::EditorSceneRenderer()
{
}

EditorSceneRenderer::~EditorSceneRenderer()
{
}

void EditorSceneRenderer::Initialise(const std::string& parHandleFileName, const std::string& parHandleMaterial)
{
    FHandleMesh = Rendering::MeshManager::Instance().CreateMesh(parHandleFileName);
    AssertRelease(FHandleMesh.IsValid());
    FHandleMaterial = Rendering::MaterialManager::CreateMaterialInstanceIFN(parHandleMaterial);
    AssertRelease(FHandleMaterial.IsValid());
}

void EditorSceneRenderer::Shutdown()
{
}

void EditorSceneRenderer::RenderScene(const Scene* parScene)
{
    Rendering::DrawCommandBuffer& commandBuffer = Rendering::BGFXRenderer::Instance().CreateCommandBuffer(0);

    AssertRelease(parScene != nullptr);
    const std::vector<std::shared_ptr<BaseSceneItem>>& sceneItems = parScene->GetSceneItems();

    foreachitemconst(sceneItem, sceneItems)
    {
        const glm::vec3& position = sceneItem->GetPosition();
        commandBuffer.DrawAABB(FHandleMaterial, position - glm::vec3(1.0f), position + glm::vec3(1.0f));

        glm::mat4 mtx = glm::translate(position);
        mtx = mtx * glm::scale(glm::vec3(0.5f)) * (glm::mat4)sceneItem->GetOrientation();

        commandBuffer.DrawMesh(FHandleMesh, FHandleMaterial, mtx);
    }
}

} // namespace ECSEngine