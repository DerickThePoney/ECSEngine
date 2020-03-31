#include "stdafx.h"

#include "ECSCoreSceneActions.h"

#include "Application/SceneItems.h"
#include "ECSCorePropertyDrawer.h"
#include "EntityFactory.h"
#include "EntityTemplateManager.h"
#include "ModuleParameters.h"

namespace ECSEngine
{

SpawnEntitySceneAction::SpawnEntitySceneAction(const std::string& parFName)
    : ISceneAction(parFName)
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
    AssertRelease(FSceneItem != nullptr);
}

void SpawnEntitySceneAction::VirtualStart()
{
    ISceneAction::VirtualStart();

    AssertRelease(FSceneItem != nullptr);

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