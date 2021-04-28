#include "stdafx.h"

#include "SceneManager.h"

namespace ECSEngine
{

void SceneManager::AddScene(const std::string parSceneFilename, const u32 position /*= -1*/)
{
    if (position >= FScenes.size())
    {
        FScenes.push_back(parSceneFilename);
    }

    FScenes.emplace(FScenes.begin() + position, parSceneFilename);
}

const std::string SceneManager::GetSceneFilenameFromIndex(const u32 parIndex) const
{
    if (parIndex > (u32)FScenes.size())
        return "";

    return FScenes[parIndex];
}

} // namespace ECSEngine
