#include "stdafx.h"

#include "InitializeTerrainSceneAction.h"

#include "Application/PropertyDrawer.h"
#include "Application/SceneActionManagement.h"
#include "Rendering/TerrainRenderer.h"

CEREAL_REGISTER_TYPE(ECSEngine::InitializeTerrainSceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::InitializeTerrainSceneAction)

namespace ECSEngine
{
IMPLEMENT_SCENE_ACTION(InitializeTerrainSceneAction);

InitializeTerrainSceneAction::InitializeTerrainSceneAction(const std::string& parName /*= "Dummy"*/)
    : parent_type(parName)
{
}

void InitializeTerrainSceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);

    Rendering::TerrainRenderer::CreateIFP();
    Rendering::TerrainRenderer::Instance().Initialize(FTerrainDescriptor, true);
}

void InitializeTerrainSceneAction::VirtualShutdown()
{
    parent_type::VirtualShutdown();

    Rendering::TerrainRenderer::Instance().Shutdown();
    Rendering::TerrainRenderer::Destroy();
}

void InitializeTerrainSceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_WITH_LIMITS("Mesh size (please make it a power of two)", FTerrainDescriptor.MeshVerticesSize, 16u, 256u);
        EDITOR_PROPERTY_WITH_LIMITS("Terrain size", FTerrainDescriptor.TerrainSize, 10.f, 2000.f);
        EDITOR_PROPERTY_WITH_LIMITS("Number of LoD levels", FTerrainDescriptor.NumberLoDLevels, 3u, 15u);
        EDITOR_PROPERTY_WITH_LIMITS("Min LoD distance", FTerrainDescriptor.MinLodDistance, 3.f, 100.f);

        if (ImGui::Button("Update parameters"))
        {
            Rendering::TerrainRenderer::Instance().Reinitialize(FTerrainDescriptor, true);
        }
    }
}

bool InitializeTerrainSceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    Rendering::TerrainRenderer::Instance().Render(); // NEED TO DO SOMETHING ABOUT THE RENDER PASSES

    return true;
}

void InitializeTerrainSceneAction::VirtualStart()
{
    parent_type::VirtualStart();
    Finish();
}

} // namespace ECSEngine