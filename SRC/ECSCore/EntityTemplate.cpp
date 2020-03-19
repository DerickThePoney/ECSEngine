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
            auto itFind = FModuleTemplates.find(i);
            if (itFind == FModuleTemplates.end())
            {
                auto& it = FModuleTemplates.emplace(i, EntityTemplateManagerMethods::CreateModuleTemplate(i));
                AssertRelease(it.first->second != nullptr);
                it.first->second->Init(this);
            }
            else
            {
                itFind->second->Init(this);
            }
        }
    }

#ifdef PERFORM_SECURITY_CHECKS
    FHasBeenInit = true;
#endif
}
} // namespace ECSEngine
