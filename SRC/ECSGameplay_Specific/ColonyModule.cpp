#include "stdafx.h"

#include "ColonyModule.h"

#include "Common/RandomGenerator.h"
#include "ECSCore/ECSCorePropertyDrawer.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"

CEREAL_REGISTER_TYPE(ECSEngine::ColonyModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ColonyModuleTemplate)

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ColonyModule, ColonyModuleTemplate);

Module* ColonyModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ColonyModule>(this, parUnitId, parParameters);
}

const EntityTemplate* ColonyModuleTemplate::PeonTemplate() const
{
    return EntityTemplateManager::Instance().GetEntityTemplate(PeonTemplateName());
}

void ColonyModuleTemplate::VirtualDrawEditor()
{
    InitialiseTemplate();
    EDITOR_PROPERTY_ENTITY_TEMPLATE("Peon template", FPeonTemplate, FPeonTemplateName);

    EDITOR_PROPERTY_SIMPLE("Starting peon quantity", FStartingPeonsNumber);
    EDITOR_PROPERTY_SIMPLE("Starting peon spawn radius", FSpawnRadius);
}

void ColonyModuleTemplate::InitialiseTemplate()
{
    if (!FPeonTemplateName.empty() || FPeonTemplate == nullptr)
        FPeonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(PeonTemplateName());
}

ColonyModule::ColonyModule()
    : Module()
{
}

namespace
{
EntityId CreateEntityInCircle(const float parMinRadius, const float parMaxRadius, const EntityTemplate* parTemplate, ModuleParameters::ParameterContainer& parContainer)
{
    AlwaysCheckedAssert(parMaxRadius >= parMinRadius);
    // Get radius
    const float randomRadius = RandomNumbers::NextFloat();
    const float radius = parMinRadius + randomRadius * (parMaxRadius - parMinRadius);

    // Get angle
    const float randomAngle = RandomNumbers::NextFloat();
    const float angle = -0.5f * glm::pi<float>() + 2.f * randomAngle * glm::pi<float>();

    // get position
    const glm::vec3 position = glm::vec3(glm::cos(angle) * radius, 0.f, glm::sin(angle) * radius);

    // spawn entity
    parContainer.Set<ModuleParameters::Position>(position);

    return EntityFactory::CreateEntity(parTemplate, parContainer);
}

void CreateNEntityInCicle(const float parMinRadius,
      const float parMaxRadius,
      const u32 parQuantity,
      const EntityTemplate* parTemplate,
      std::vector<EntityId>& outCreatedEntities,
      ModuleParameters::ParameterContainer& parContainer)
{
    forrange(i, 0, parQuantity) { outCreatedEntities.push_back(CreateEntityInCircle(parMinRadius, parMaxRadius, parTemplate, parContainer)); }
}
} // namespace
void ColonyModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

    FName = fmt::format("Colony_{}", parUnitId.GetSequentialId());

    const EntityTemplate* peonTemplate = Template<ColonyModuleTemplate>()->PeonTemplate();
    AssertRelease(peonTemplate != nullptr);
    AssertRelease(peonTemplate->GetWorldId() == Worlds::PEONS);
    std::vector<EntityId> createdEntities;

    ModuleParameters::ParameterContainer container;
    container.Set<ModuleParameters::OwnerId>(parUnitId);

    createdEntities.reserve(Template<ColonyModuleTemplate>()->StartingPeonNumber());
    CreateNEntityInCicle(Template<ColonyModuleTemplate>()->SpawnRadius(), Template<ColonyModuleTemplate>()->SpawnRadius(), Template<ColonyModuleTemplate>()->StartingPeonNumber(),
          peonTemplate, createdEntities, container);
}

} // namespace ECSEngine
