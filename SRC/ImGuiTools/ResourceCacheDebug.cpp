#include "stdafx.h"

#include "ResourceCacheDebug.h"

#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"

namespace ECSEngine
{
namespace ImGUITools
{

void DrawResourceCacheDebug(bool* parOpen)
{
    ImGui::Begin("Resource cache", parOpen);

    if (ImGui::CollapsingHeader("Global stats", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("File system type: %s", GlobalResourceCache::Instance().FCache->GetFileSystemInfo());
        ImGui::Text("Allocated: %.2f / %.2f MB", (float)GlobalResourceCache::Instance().FCache->Allocated() / 1024.f / 1024.f,
              (float)GlobalResourceCache::Instance().FCache->CacheSize() / 1024.f / 1024.f);
    }

    if (ImGui::CollapsingHeader("Allocated Resources", ImGuiTreeNodeFlags_DefaultOpen))
    {
        foreachitemconst(res, GlobalResourceCache::Instance().FCache->AllocatedResources())
        {
            ImGui::Text("%s: %.2f MB (%d bytes)", res.first.c_str(), (float)res.second->Size() / 1024.f / 1024.f, res.second->Size());
        }
    }
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
