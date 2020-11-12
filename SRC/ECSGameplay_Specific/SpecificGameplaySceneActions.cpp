#include "stdafx.h"

#include "SpecificGameplaySceneActions.h"

#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSGameplay_Common/ApparenceModule.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

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
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Fire place template", FFirePlaceTemplate, FWorldParameters.FFirePlaceTemplateName);
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
    FFirePlaceTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FWorldParameters.FFirePlaceTemplateName);
}

void CreateWorldSceneAction::VirtualShutdown()
{
    parent_type::VirtualShutdown();
}

void CreateWorldSceneAction::VirtualStart()
{
    parent_type::VirtualStart();
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
