#include "stdafx.h"

#include "SpecificGameplaySceneActions.h"

#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"
#include "WorldBuilder.h"

CEREAL_REGISTER_TYPE(ECSEngine::CreateWorldSceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::CreateWorldSceneAction)

namespace ECSEngine
{

IMPLEMENT_SCENE_ACTION(CreateWorldSceneAction);

CreateWorldSceneAction::CreateWorldSceneAction(const std::string& parName /*= "Dummy"*/)
    : ISceneAction(parName)
{
}

void CreateWorldSceneAction::VirtualDrawEditor()
{
    parent_type::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Colony template", FColonyTemplate, FWorldParametersDescriptor.FColonyTemplateName);
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Fire place template", FFirePlaceTemplate, FWorldParametersDescriptor.FFirePlaceTemplateName);
        ImGui::Separator();
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Food template", FFoodTemplate, FWorldParametersDescriptor.FFoodTemplateName);
        EDITOR_PROPERTY_WITH_LIMITS("Min radius for spawn", FWorldParametersDescriptor.FMinFoodRadius, 0.f, FWorldParametersDescriptor.FMaxFoodRadius);
        EDITOR_PROPERTY_WITH_LIMITS("Max radius for spawn", FWorldParametersDescriptor.FMaxFoodRadius, FWorldParametersDescriptor.FMinFoodRadius, 10000.0f);
        EDITOR_PROPERTY_WITH_LIMITS("Nb food to spawn", FWorldParametersDescriptor.FNbFoodEntities, 0u, 200u);
        ImGui::Separator();
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Peon template", FPeonTemplate, FWorldParametersDescriptor.FPeonTemplateName);
        EDITOR_PROPERTY_WITH_LIMITS("Radius for peons spawn", FWorldParametersDescriptor.FSpawnRadius, 0.f, FWorldParametersDescriptor.FSpawnRadius);
        EDITOR_PROPERTY_WITH_LIMITS("Nb peons to spawn at start", FWorldParametersDescriptor.FStartingPeonsNumber, 0u, 20u);
    }
}

bool CreateWorldSceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    parent_type::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);

    if (FFirePlaceTemplate != nullptr)
    {
        const bool hasApparenceModule = FFirePlaceTemplate->HasModule<ApparenceModule>();
        if (!hasApparenceModule)
            return false;

        const ApparenceModuleTemplate* apparenceModuleTemplate = (ApparenceModuleTemplate*)FFirePlaceTemplate->GetModuleTemplate<ApparenceModule>();
        AssertRelease(apparenceModuleTemplate != nullptr);
        const Rendering::MeshHandle meshHandle = Rendering::MeshManager::Instance().CreateMesh(apparenceModuleTemplate->GetMeshFileName());
        const Rendering::MaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(apparenceModuleTemplate->GetMaterialFileName());
        if (!meshHandle.IsValid())
            return false;

        parCommandBuffer.DrawMesh(meshHandle, (instanceHandle.IsValid()) ? instanceHandle : parMaterial);
    }

    return true;
}
void CreateWorldSceneAction::VirtualInitialise(const SceneScenario* parScene)
{
    parent_type::VirtualInitialise(parScene);
    FColonyTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FWorldParametersDescriptor.FColonyTemplateName);
    FFirePlaceTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FWorldParametersDescriptor.FFirePlaceTemplateName);
    FFoodTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FWorldParametersDescriptor.FFoodTemplateName);
    FPeonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FWorldParametersDescriptor.FPeonTemplateName);
}

void CreateWorldSceneAction::VirtualShutdown()
{
    parent_type::VirtualShutdown();
}

void CreateWorldSceneAction::VirtualStart()
{
    parent_type::VirtualStart();
    WorldBuilder worldBuilder(FWorldParametersDescriptor);
    worldBuilder.CreateWorld();
    Finish();
}

void CreateWorldSceneAction::VirtualUpdate()
{
    parent_type::VirtualUpdate();
}

void CreateWorldSceneAction::VirtualFinish()
{
    parent_type::VirtualFinish();
}

} // namespace ECSEngine
