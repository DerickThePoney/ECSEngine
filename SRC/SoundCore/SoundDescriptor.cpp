#include "stdafx.h"

#include "SoundDescriptor.h"

#include "Application/PropertyDrawer.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(SoundDescriptor);

void SoundDescriptor::DrawInEditor()
{
    EDITOR_PROPERTY_STRING("Sound filename", FFilename, true, "*.wav", SoundResourceCache::Instance().FCache->GetFileSystem());

    i32 selectedIdx = (i32)FSoundGroup;

    if (ImGui::BeginCombo("Sound group", SoundGroup::GetName(FSoundGroup)))
    {
        forrange(i, 0, (i32)ESoundGroup::LENGTH)
        {
            if (ImGui::Selectable(SoundGroup::GetName((ESoundGroup)i), i == selectedIdx))
            {
                FSoundGroup = (ESoundGroup)i;
                break;
            }
        }
        ImGui::EndCombo();
    }

    EDITOR_PROPERTY_BOOL("Stream sound", FStream);
    EDITOR_PROPERTY_BOOL("Spatialized", FSpatialized);
    EDITOR_PROPERTY_BOOL("Allow Pitch changes", FAllowPitchChanges);
    EDITOR_PROPERTY_BOOL("Loop sound", FbLoop);
}

} // namespace ECSEngine