
#include "stdafx.h"

#include "RecipeProductionModule.h"

#include "Common/SavingSystemImplementation.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleUtils.h"
#include "ECSGameplaySpecificPropertyDrawers.h"
#include "GameplayRulesManager.h"
#include "ProductionRecipesManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::RecipeProductionModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::RecipeProductionModuleTemplate);

namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(RecipeProductionModule, RecipeProductionModuleTemplate);

Module* RecipeProductionModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<RecipeProductionModule>(this, parUnitId, parParameters);
}

const ProductionRecipe* RecipeProductionModuleTemplate::GetProductionRecipe() const
{
    if (FRecipe != nullptr)
    {
        return FRecipe;
    }

    return GameplayRulesManager::Instance().FProductionRecipesManager.GetProductionRecipe(FRecipeName);
}

void RecipeProductionModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_PRODUCTION_RECIPE("Recipe", FRecipe, FRecipeName);
}

void RecipeProductionModuleTemplate::VirtualPostLoad()
{
    ModuleTemplate::VirtualPostLoad();
}

IMPLEMENT_SAVELOAD_ABILITIES_FREEFUNC(RecipeProductionState::Type);

IMPLEMENT_SAVELOAD_ABILITIES(RecipeProductionModule);
template<typename Chunk, bool isWriting>
void RecipeProductionModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
    parChunk& FState;
    parChunk& FProductionTimeRemaining;
}

RecipeProductionModule::RecipeProductionModule()
    : Module()
{
}

RecipeProductionModule::~RecipeProductionModule()
{
}

const ProductionRecipe* RecipeProductionModule::GetProductionRecipe() const
{
    const RecipeProductionModuleTemplate* temp = Template<RecipeProductionModuleTemplate>();
    AssertRelease(temp != nullptr);
    return temp->GetProductionRecipe();
}

} // namespace ECSEngine
