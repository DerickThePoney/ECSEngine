#include "stdafx.h"

#include "ECSCoreSceneActions.h"

#include "Application/SceneItems.h"
#include "ECSCorePropertyDrawer.h"
#include "EntityFactory.h"
#include "EntityTemplateManager.h"
#include "ModuleParameters.h"

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
    // container.Set<ModuleParameters::Orientation>(FSceneItem->GetEulerAngles());

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

} // namespace ECSEngine