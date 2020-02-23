#include "stdafx.h"

#include "EntityTemplateManager.h"

namespace ECSEngine
{
EntityTemplate* EntityTemplateManager::CreateNewEntityTemplate()
{
    FEntityTemplates.push_back(EntityTemplate());
    return &FEntityTemplates[FEntityTemplates.size() - 1];
}

const EntityTemplate* EntityTemplateManager::GetEntityTemplate(u32 parIndex)
{
    AssertRelease(parIndex < FEntityTemplates.size());
    return &FEntityTemplates[parIndex];
}

namespace EntityTemplateManagerMethods
{
static std::unordered_map<u32, ModuleTemplate* (*)()> FModuleTemplateFactories;
bool RegisterTemplateFactory(const u32 parId, ModuleTemplate* (*parFactory)())
{
    AlwaysCheckedAssert(FModuleTemplateFactories.find(parId) == FModuleTemplateFactories.end());
    FModuleTemplateFactories[parId] = parFactory;
    return true;
}

ModuleTemplate* CreateModuleTemplate(const u32 parId)
{
    AlwaysCheckedAssert(FModuleTemplateFactories.find(parId) != FModuleTemplateFactories.end());
    ModuleTemplate* temp = FModuleTemplateFactories[parId]();
    AssertRelease(temp != nullptr);
    return temp;
}
} // namespace EntityTemplateManagerMethods

} // namespace ECSEngine