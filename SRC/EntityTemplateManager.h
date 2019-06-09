#pragma once
#include "EntityTemplate.h"
#include "Singleton.h"

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

private:
    std::vector<EntityTemplate> FEntityTemplates;
};
} // namespace ECSEngine