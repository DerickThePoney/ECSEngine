#include "stdafx.h"

#include "ECSCoreSceneActions.h"

#include "Application/SceneItems.h"
#include "ECSCorePropertyDrawer.h"
#include "EntityFactory.h"
#include "EntityTemplateManager.h"
#include "ModuleParameters.h"

CEREAL_REGISTER_TYPE(ECSEngine::SpawnEntitySceneAction);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ISceneAction, ECSEngine::SpawnEntitySceneAction)

namespace ECSEngine
{
IMPLEMENT_SCENE_ACTION(SpawnEntitySceneAction);

SpawnEntitySceneAction::SpawnEntitySceneAction(const std::string& parFName)
    : ISceneAction(parFName)
{
}

SpawnEntitySceneAction::~SpawnEntitySceneAction()
{
}

void SpawnEntitySceneAction::VirtualInitialise(const Scene* parScene)
{
    ISceneAction::VirtualInitialise(parScene);
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
    auto sceneItems = GetScene()->GetSceneItems();
    if (FSceneItemID < sceneItems.size())
    {
        FSceneItem = sceneItems[FSceneItemID].get();
        AssertRelease(FSceneItem != nullptr);
    }
}

void SpawnEntitySceneAction::VirtualStart()
{
    ISceneAction::VirtualStart();

    AssertRelease(FSceneItem != nullptr);
    AssertRelease(FTemplate != nullptr);

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(FSceneItem->GetPosition());
    // container.Set<ModuleParameters::Orientation>(FSceneItem->GetEulerAngles());

    EntityFactory::CreateEntity(FTemplate, container);
}

void SpawnEntitySceneAction::VirtualDrawEditor()
{
    ISceneAction::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Entity to spawn", FTemplate, FEntityTemplateName);
        EDITOR_PROPERTY_SCENE_ITEM("Scene item", FSceneItem, FSceneItemID, GetScene());
    }
}

void SpawnEntitySceneAction::SetSceneItem(const BaseSceneItem* parSceneItem)
{
    FSceneItem = parSceneItem;
    FSceneItemID = parSceneItem->GetSceneItemTypeId();
}

} // namespace ECSEngine