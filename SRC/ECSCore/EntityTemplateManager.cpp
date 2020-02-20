#include "stdafx.h"

#include "EntityTemplateManager.h"

namespace ECSEngine
{
EntityTemplate* EntityTemplateManager::CreateNewEntityTemplate()
{
    FEntityTemplates.push_back(EntityTemplate());
    return &FEntityTemplates[FEntityTemplates.size() - 1];
}
} // namespace ECSEngine