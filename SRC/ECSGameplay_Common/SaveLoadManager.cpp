#include "stdafx.h"

#include "SaveLoadManager.h"

#include "Common/SaveLoadData.h"
#include "Common/TimeManager.h"

#include <filesystem>
#include <fstream>

namespace ECSEngine
{

void SaveLoadManager::Initialise()
{
    FSaveFolder = getenv("LOCALAPPDATA");
    FSaveFolder += "\\ECSEngine";
    std::filesystem::create_directory(FSaveFolder);
    FSaveFolder += "\\SavedGames\\";
    std::filesystem::create_directory(FSaveFolder);
}

void SaveLoadManager::Cleanup()
{
}

void SaveLoadManager::Update()
{
    FAutoSaveTimer -= TimeManager::FrameDeltaTime();
    if (FAutoSaveTimer <= 0.f && (!FRequestSave && !FRequestQuickSave))
    {
        FRequestAutoSave = true;
    }
}

void SaveLoadManager::RequestSave(const std::string& parFilename)
{
    AlwaysCheckedAssert(!FRequestLoad);
    FRequestSave = true;
    FSaveFilename = parFilename;
}

void SaveLoadManager::RequestLoad(const std::string& parFilename)
{
    AlwaysCheckedAssert(!FRequestSave);
    FRequestLoad = true;
    FSaveFilename = parFilename;
}

void SaveLoadManager::RequestQuickSave()
{
    AlwaysCheckedAssert(!FRequestQuickLoad);
    FRequestQuickSave = true;
}

void SaveLoadManager::RequestQuickLoad()
{
    AlwaysCheckedAssert(!FRequestQuickSave);
    FRequestQuickLoad = true;
}

} // namespace ECSEngine