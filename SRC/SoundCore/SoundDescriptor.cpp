#include "stdafx.h"

#include "SoundDescriptor.h"

#include "Application/PropertyDrawer.h"

namespace ECSEngine
{
IMPLEMENT_POOL_ALLOCATED(SoundDescriptor);

void SoundDescriptor::DrawInEditor()
{
    EDITOR_PROPERTY_STRING("Sound filename", FFilename, true, "*.wav");

    // todo enum

    EDITOR_PROPERTY_BOOL("Stream sound", FStream);
    EDITOR_PROPERTY_BOOL("Spatialized", FSpatialized);
    EDITOR_PROPERTY_BOOL("Allow Pitch changes", FAllowPitchChanges);
    EDITOR_PROPERTY_BOOL("Loop sound", FbLoop);
}

} // namespace ECSEngine