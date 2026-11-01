
#pragma once
#include "ECSCore/Module.h"
#include "ECSCore/ModuleTemplate.h"
#include "ProductionRecipe.h"

namespace ECSEngine
{
class RecipeProductionModuleTemplate : public ModuleTemplate
{
    DECLARE_MODULE_TEMPLATE(RecipeProductionModule, RecipeProductionModuleTemplate);

public:
    RecipeProductionModuleTemplate()
        : ModuleTemplate()
    {
    }
    ~RecipeProductionModuleTemplate() { }

    Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const override;

    SERIALIZE() { PROPERTYFIELD(RecipeName, ""); }

    const ProductionRecipe* GetProductionRecipe() const;

protected:
    void VirtualDrawEditor() override;
    void VirtualPostLoad() override;

private:
    std::string FRecipeName = "";
    const ProductionRecipe* FRecipe = nullptr;
};

namespace RecipeProductionState
{
enum Type
{
    IDLE,
    PRODUCING
};
}

class RecipeProductionModule : public Module
{
    DECLARE_MODULE(RecipeProductionModule);

    DECLARE_SAVELOAD_ABILITIES();

public:
    RecipeProductionModule();
    ~RecipeProductionModule();

    RecipeProductionState::Type State() const { return FState; }
    void SetState(RecipeProductionState::Type parState) { FState = parState; }

    float ProductionTimeRemaining() const { return FProductionTimeRemaining; }
    void SetProductionTimeRemaining(float parTime) { FProductionTimeRemaining = parTime; }

    const ProductionRecipe* GetProductionRecipe() const;

private:
    RecipeProductionState::Type FState = RecipeProductionState::IDLE;
    float FProductionTimeRemaining = 0.f;
};

} // namespace ECSEngine
