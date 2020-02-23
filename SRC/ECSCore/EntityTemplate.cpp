#include "stdafx.h"

#include "EntityTemplate.h"

#include "EntityTemplateManager.h"

namespace ECSEngine
{
void EntityTemplate::Initialise()
{
    for (u32 i = 0; i < (u32)EModuleId::Length; ++i)
    {
        if (HasModule(i))
        {
            ModuleTemplate* modTemp = EntityTemplateManagerMethods::CreateModuleTemplate(i);
            AssertRelease(modTemp != nullptr);
            modTemp->Init(this);
            FModuleTemplates[i] = modTemp;
        }
    }

#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}

} // namespace ECSEngine
