#pragma once
#include "Common/Singleton.h"
#include "EntityTemplate.h"

namespace ECSEngine
{
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