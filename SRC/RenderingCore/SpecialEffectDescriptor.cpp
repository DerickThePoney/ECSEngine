#include "stdafx.h"

#include "SpecialEffectDescriptor.h"

#include "SoundCore/SoundDescriptor.h"
#include "VisualEffectDescriptor.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(SpecialEffectDescriptor);

void SpecialEffectDescriptor::DrawInEditor()
{
    ImGui::PushID(ImGui::GetID(this));
    if (FVFXDescriptor != nullptr)
    {
        if (ImGui::Button("Remove visual effect"))
        {
            FVFXDescriptor = nullptr;
        }
        else
        {
            if (ImGui::CollapsingHeader("Visual effect"))
            {
                // Draw vfx
            }
        }
    }
    else
    {
        if (ImGui::Button("Add visual effect"))
        {
            FVFXDescriptor.reset(new VisualEffectDescriptor());
        }
    }

    if (FSoundDescriptor != nullptr)
    {
        if (ImGui::Button("Remove sound effect"))
        {
            FSoundDescriptor = nullptr;
        }
        else
        {
            if (ImGui::CollapsingHeader("Sound effect"))
            {
                FSoundDescriptor->DrawInEditor();
            }
        }
    }
    else
    {
        if (ImGui::Button("Add sound effect"))
        {
            FSoundDescriptor.reset(new SoundDescriptor());
        }
    }

    ImGui::PopID();
}

} // namespace ECSEngine
