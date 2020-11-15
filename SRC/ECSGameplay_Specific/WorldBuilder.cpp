#include "stdafx.h"

#include "WorldBuilder.h"

#include "ColonyPeonsManagementModule.h"
#include "Common/RandomGenerator.h"
#include "ECSCore/EntityFactory.h"
#include "ECSCore/EntityId.h"
#include "ECSCore/EntityTemplate.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/WorldIds.h"

namespace ECSEngine
{
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

WorldBuilder::WorldBuilder(const WorldGenerationParametersDescriptor& parWorldGenerationParameters)
    : FGenerationParameters(parWorldGenerationParameters)
{
}

void WorldBuilder::CreateWorld() const
{
    // Create colony
    const EntityTemplate* colonyTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FGenerationParameters.FColonyTemplateName);
    AssertRelease(colonyTemplate != nullptr);
    AssertRelease(colonyTemplate->GetWorldId() == Worlds::COLONY);
    ModuleParameters::ParameterContainer colonyContainer;
    colonyContainer.Set<ModuleParameters::Position>(glm::vec3(0.f));
    const EntityId colonyId = EntityFactory::CreateEntity(colonyTemplate, colonyContainer);

    const EntityTemplate* firePlaceTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FGenerationParameters.FFirePlaceTemplateName);
    AssertRelease(firePlaceTemplate != nullptr);
    AssertRelease(firePlaceTemplate->GetWorldId() == Worlds::STANDARD);
    ModuleParameters::ParameterContainer firePlaceContainer;
    firePlaceContainer.Set<ModuleParameters::Position>(glm::vec3(0.f));
    const EntityId firePlaceId = EntityFactory::CreateEntity(firePlaceTemplate, firePlaceContainer);

    const EntityTemplate* foodTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FGenerationParameters.FFoodTemplateName);
    AssertRelease(foodTemplate != nullptr);
    AssertRelease(foodTemplate->GetWorldId() == Worlds::RESOURCE_PROD);
    std::vector<EntityId> createdEntities;
    createdEntities.reserve(FGenerationParameters.FNbFoodEntities);
    ModuleParameters::ParameterContainer foodParamContainer;
    CreateNEntityInCicle(
          FGenerationParameters.FMinFoodRadius, FGenerationParameters.FMaxFoodRadius, FGenerationParameters.FNbFoodEntities, foodTemplate, createdEntities, foodParamContainer);

    const EntityTemplate* peonTemplate = EntityTemplateManager::Instance().GetEntityTemplate(FGenerationParameters.FPeonTemplateName);
    AssertRelease(peonTemplate != nullptr);
    AssertRelease(peonTemplate->GetWorldId() == Worlds::PEONS);
    std::vector<EntityId> createdPeons;
    createdPeons.reserve(FGenerationParameters.FStartingPeonsNumber);
    ModuleParameters::ParameterContainer peonsParamContainer;
    peonsParamContainer.Set<ModuleParameters::OwnerId>(colonyId);
    CreateNEntityInCicle(
          FGenerationParameters.FSpawnRadius, FGenerationParameters.FSpawnRadius, FGenerationParameters.FStartingPeonsNumber, peonTemplate, createdPeons, peonsParamContainer);

    {
        ManualLockModuleAccessor<ColonyPeonsManagementModule> colonyPeonsManagementModuleAccessor(Worlds::COLONY);
        colonyPeonsManagementModuleAccessor.LockIFN();
        ColonyPeonsManagementModule* colonyPeonsModule = colonyPeonsManagementModuleAccessor[colonyId];
        AssertRelease(colonyPeonsModule != nullptr);
        colonyPeonsManagementModuleAccessor.UnlockIFN();

        colonyPeonsModule->IdlePeons().assign(createdPeons.begin(), createdPeons.end());
    }
}

} // namespace ECSEngine