#pragma once
#include "Common/Singleton.h"
#include "EntityTemplate.h"

namespace ECSEngine
{
namespace EntityTemplateManagerMethods
{
bool RegisterTemplateFactory(const u32 parId, ModuleTemplate* (*parFactory)());
ModuleTemplate* CreateModuleTemplate(const u32 parId);
} // namespace EntityTemplateManagerMethods

class EntityTemplateManager final : public Singleton<EntityTemplateManager>
{
public:
    EntityTemplateManager()
        : Singleton<EntityTemplateManager>()
    {
    }
    EntityTemplate* CreateNewEntityTemplate();
    const EntityTemplate* GetEntityTemplate(u32 parIndex);

private:
    std::vector<EntityTemplate> FEntityTemplates;
};
} // namespace ECSEngine