#include "stdafx.h"

#include "EntityTemplateManager.h"

#include "EntityTemplate.h"
#include "EntityTemplateManagerMethods.h"
#include "ModuleTemplate.h"

namespace ECSEngine
{
EntityTemplate* EntityTemplateManager::CreateNewEntityTemplate()
{
    FEntityTemplates.push_back(std::shared_ptr<EntityTemplate>(new EntityTemplate()));
    return FEntityTemplates[FEntityTemplates.size() - 1].get();
}

const EntityTemplate* EntityTemplateManager::GetEntityTemplate(u32 parIndex) const
{
    AssertRelease(parIndex < FEntityTemplates.size());
    return FEntityTemplates[parIndex].get();
}

const ECSEngine::EntityTemplate* EntityTemplateManager::GetEntityTemplate(const std::string& parTemplateName) const
{
    std::string lowered(parTemplateName.size(), ' ');
    std::transform(parTemplateName.begin(), parTemplateName.end(), lowered.begin(), [](uc8 c) { return std::tolower(c); });
    foreachitemconst(entityTemplate, FEntityTemplates)
    {
        if (entityTemplate == nullptr)
            continue;
        const std::string& templateName = entityTemplate->GetName();
        std::string loweredTemplate(templateName.size(), ' ');
        std::transform(templateName.begin(), templateName.end(), loweredTemplate.begin(), [](uc8 c) { return std::tolower(c); });
        if (loweredTemplate == lowered)
        {
            return entityTemplate.get();
        }
    }
    return nullptr;
}

EntityTemplate* EntityTemplateManager::GetEntityTemplateForWriting(u32 parIndex)
{
    AssertRelease(parIndex < FEntityTemplates.size());
    return FEntityTemplates[parIndex].get();
}

void EntityTemplateManager::DeleteEntityTemplate_IKNOWWHATIMDOING(u32 parIndex)
{
    AssertRelease(parIndex < FEntityTemplates.size());
    FEntityTemplates.erase(FEntityTemplates.begin() + parIndex);
}

EntityTemplateManager::~EntityTemplateManager()
{
}

void EntityTemplateManager::InitAfterLoad()
{
    foreachitemconst(et, FEntityTemplates) { et->Initialise(); }
}

namespace EntityTemplateManagerMethods
{
static std::unordered_map<u32, ModuleTemplate* (*)()>* FModuleTemplateFactories = nullptr;
static std::map<u32, std::string>* FModuleList = nullptr;
bool RegisterTemplateFactory(const u32 parId, ModuleTemplate* (*parFactory)())
{
    if (FModuleTemplateFactories == nullptr)
        FModuleTemplateFactories = new std::unordered_map<u32, ModuleTemplate* (*)()>();
    if (FModuleList == nullptr)
        FModuleList = new std::map<u32, std::string>();

    AlwaysCheckedAssert(FModuleTemplateFactories->find(parId) == FModuleTemplateFactories->end());
    (*FModuleTemplateFactories)[parId] = parFactory;
    ModuleTemplate* temp = CreateModuleTemplate(parId);
    AlwaysCheckedAssert(FModuleList->find(parId) == FModuleList->end());
    (*FModuleList)[parId] = temp->GetName();
    delete temp;
    return true;
}

ModuleTemplate* CreateModuleTemplate(const u32 parId)
{
    AlwaysCheckedAssert(FModuleTemplateFactories->find(parId) != FModuleTemplateFactories->end());
    ModuleTemplate* temp = (*FModuleTemplateFactories)[parId]();
    AssertRelease(temp != nullptr);
    return temp;
}

const std::map<u32, std::string>& GetModuleList()
{
    return *FModuleList;
}

void Cleanup()
{
    delete FModuleTemplateFactories;
    FModuleTemplateFactories = nullptr;
    delete FModuleList;
    FModuleList = nullptr;
}

} // namespace EntityTemplateManagerMethods

} // namespace ECSEngine
