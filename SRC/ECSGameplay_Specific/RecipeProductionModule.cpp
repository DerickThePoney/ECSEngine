
#include "stdafx.h"

#include "RecipeProductionModule.h"

#include "ECSCore/EntityTemplateManager.h"
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

RecipeProductionModule::RecipeProductionModule()
    : Module()
{
}

RecipeProductionModule::~RecipeProductionModule()
{
}

} // namespace ECSEngine
