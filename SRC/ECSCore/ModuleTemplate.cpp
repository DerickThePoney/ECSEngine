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

bool ModuleTemplate::DrawEditor()
{
    static bool open = false;
    if (ImGui::CollapsingHeader(GetName().c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
    {
        if (ImGui::Button("Delete module"))
        {
            return true;
        }
        VirtualDrawEditor();
    }

    return false;
}
#ifdef PERFORM_SECURITY_CHECKS
void ModuleTemplate::VerifyTemplate()
{
    VirtualVerifyTemplate();
}
#endif

} // namespace ECSEngine