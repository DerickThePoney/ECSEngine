#include "stdafx.h"

#include "EntityTemplateManager.h"

namespace ECSEngine
{
EntityTemplate* EntityTemplateManager::CreateNewEntityTemplate()
{
    FEntityTemplates.push_back(EntityTemplate());
    return &FEntityTemplates[FEntityTemplates.size() - 1];
}

const ECSEngine::EntityTemplate* EntityTemplateManager::GetEntityTemplate(u32 parIndex)
{
    AssertRelease(parIndex < FEntityTemplates.size());
    return &FEntityTemplates[parIndex];
}

} // namespace ECSEngine