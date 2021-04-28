#pragma once
#include "Common/MemoryView.h"
#include "Common/Singleton.h"

namespace ECSEngine
{
class SceneManager : public Singleton<SceneManager>
{
public:
    void AddScene(const std::string parSceneFilename, const u32 position = -1);

    const std::string GetSceneFilenameFromIndex(const u32 parIndex) const;
    MemoryView<const std::string> Scenes() const { return MemoryView(FScenes.data(), (u32)FScenes.size()); }

    std::vector<std::string>& ScenesForWriting() { return FScenes; }

    SERIALIZE() { PROPERTYFIELD(Scenes, std::vector<std::string>()); }

private:
    std::vector<std::string> FScenes;
};
} // namespace ECSEngine
