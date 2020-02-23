#include "stdafx.h"

#include "ModuleTemplate.h"

namespace ECSEngine
{
ModuleTemplate::ModuleTemplate()
    : FTemplate(nullptr)
#ifdef PERFORM_SECURITY_CHECKS
    , FHasBeenInit(false)
#endif
{
}

ModuleTemplate::~ModuleTemplate()
{
}

void ModuleTemplate::Init(const EntityTemplate* parTemplate)
{
    FTemplate = parTemplate;
#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}
} // namespace ECSEngine