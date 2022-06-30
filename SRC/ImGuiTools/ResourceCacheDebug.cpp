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
        const ResourceHandleMap& resources = GlobalResourceCache::Instance().FCache->AllocatedResources();

        struct ResourceRecord
        {
            std::string Name;
            float sizeMB;
            u32 sizeB;
        };
        std::vector<ResourceRecord> records;
        records.reserve(resources.size());

        foreachitemconst(res, resources) { records.emplace_back(ResourceRecord{ res.first, (float)res.second->Size() / 1024.f / 1024.f, res.second->Size() }); }

        std::sort(records.begin(), records.end(), [](ResourceRecord& a, ResourceRecord& b) { return a.sizeB > b.sizeB; });

        foreachitemconst(rec, records) { ImGui::Text("%s: %.2f MB (%d bytes)", rec.Name.c_str(), rec.sizeMB, rec.sizeB); }
    }
    ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine
