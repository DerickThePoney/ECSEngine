#include "stdafx.h"

#include "ModuleTemplate.h"

ECSEngine::ModuleTemplate::ModuleTemplate()
    : FTemplate(nullptr)
#ifdef PERFORM_SECURITY_CHECKS
    , FHasBeenInit(false)
#endif
{
}

ECSEngine::ModuleTemplate::~ModuleTemplate()
{
}

void ECSEngine::ModuleTemplate::Init(const EntityTemplate* parTemplate)
{
    FTemplate = parTemplate;
#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}
