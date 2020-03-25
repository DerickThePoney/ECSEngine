#include "stdafx.h"

#include "ECSCoreSceneActions.h"

#include "Application/SceneItems.h"
#include "ECSCorePropertyDrawer.h"
#include "EntityFactory.h"
#include "EntityTemplateManager.h"
#include "ModuleParameters.h"

namespace ECSEngine
{

SpawnEntitySceneAction::SpawnEntitySceneAction()
{
}

SpawnEntitySceneAction::~SpawnEntitySceneAction()
{
}

void SpawnEntitySceneAction::VirtualInitialise()
{
    ISceneAction::VirtualInitialise();
    FTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FEntityTemplateName);
    AssertRelease(FTemplate != nullptr);
}

void SpawnEntitySceneAction::VirtualStart()
{
    ISceneAction::VirtualStart();

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::Position>(FSceneItem->GetPosition());
    container.Set<ModuleParameters::Orientation>(FSceneItem->GetOrientation());

    EntityFactory::CreateEntity(FTemplate, container);
}

void SpawnEntitySceneAction::VirtualDrawEditor()
{
    ISceneAction::VirtualDrawEditor();

    if (ShouldShowEditor())
    {
        EDITOR_PROPERTY_ENTITY_TEMPLATE("Entity to spawn", FTemplate, FEntityTemplateName);
    }
}

} // namespace ECSEngine