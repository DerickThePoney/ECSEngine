#include "stdafx.h"

#include "GameplaySceneActions.h"

#include "ApparenceModule.h"
#include "Application/SceneItems.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::SpawnEntitySceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::SceneActionWithBaseSceneItem, ECSEngine::SpawnEntitySceneAction)

namespace ECSEngine
{
IMPLEMENT_SCENE_ACTION(SpawnEntitySceneAction);

SpawnEntitySceneAction::SpawnEntitySceneAction(const std::string& parFName)
    : SceneActionWithBaseSceneItem(parFName)
{
}

SpawnEntitySceneAction::~SpawnEntitySceneAction()
{
}

void SpawnEntitySceneAction::VirtualInitialise(const Scene* parScene)
{
    SceneActionWithBaseSceneItem::VirtualInitialise(parScene);
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
}

void SpawnEntitySceneAction::VirtualStart()
{
    SceneActionWithBaseSceneItem::VirtualStart();

    AssertRelease(FSceneItem != nullptr);
    AssertRelease(FTemplate != nullptr);

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(FSceneItem->GetPosition());
    container.Set<ModuleParameters::EulerAngles>(FSceneItem->GetEulerAngles());

    EntityFactory::CreateEntity(FTemplate, container);
}

void SpawnEntitySceneAction::VirtualDrawEditor()
{
    SceneActionWithBaseSceneItem::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Entity to spawn", FTemplate, FEntityTemplateName);
    }
}

bool SpawnEntitySceneAction::VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial)
{
    SceneActionWithBaseSceneItem::VirtualDrawInSceneEditor(parCommandBuffer, parMaterial);
    const bool hasApparenceModule = FTemplate->HasModule<ApparenceModule>();
    if (!hasApparenceModule)
        return false;

    const BaseSceneItem* item = GetSceneItem();
    if (item == nullptr)
        return false;

    const ApparenceModuleTemplate* apparenceModuleTemplate = (ApparenceModuleTemplate*)FTemplate->GetModuleTemplate<ApparenceModule>();
    AssertRelease(apparenceModuleTemplate != nullptr);
    const Rendering::MeshHandle meshHandle = Rendering::MeshManager::Instance().CreateMesh(apparenceModuleTemplate->GetMeshFileName());
    const Rendering::MaterialInstanceHandle instanceHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(apparenceModuleTemplate->GetMaterialFileName());
    if (meshHandle.IsValid())
    {
        const glm::vec3 eulerAngles = item->GetEulerAngles();
        const glm::mat4 mtx = glm::translate(item->GetPosition()) * glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z);

        parCommandBuffer.DrawMesh(meshHandle, (instanceHandle.IsValid()) ? instanceHandle : parMaterial, mtx);
    }

    return true;
}

} // namespace ECSEngine