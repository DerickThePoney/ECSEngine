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

void SaveLoadManager::HandleSaveLoad(GameScenarioUpdater* sceneScenario)
{
    AlwaysCheckedAssert(!(FRequestSave && FRequestLoad));
    AlwaysCheckedAssert(!(FRequestQuickSave && FRequestQuickLoad));

    if (FRequestSave || FRequestQuickSave || FRequestAutoSave)
    {
        SavingSystem::SaveChunk chunk;
        chunk&(*sceneScenario);

        std::string filename = (FRequestSave) ? FSaveFilename : (FRequestQuickSave) ? "QuickSave.sav" : "AutoSave.sav";

        std::ofstream ofstr(FSaveFolder + filename, std::ios::binary);
        AssertRelease(ofstr.good());
        if (ofstr.good())
            ofstr.write(reinterpret_cast<const char*>(chunk.GetBuffer().Data()), chunk.GetBuffer().WrittenBytes());

        FRequestSave = false;
        FRequestQuickSave = false;
        FRequestAutoSave = false;
    }
    else if (FRequestLoad || FRequestQuickLoad)
    {
        std::string filename = (FRequestSave) ? FSaveFilename : "QuickSave.sav";
        SavingSystem::ReadChunk rc;

        {
            char* data = nullptr;
            u32 length = 0;

            std::ifstream ifstr(FSaveFolder + filename, std::ios::binary);
            AssertRelease(ifstr.good());
            if (ifstr.good())
            {
                ifstr.seekg(0, ifstr.end);
                length = ifstr.tellg();
                ifstr.seekg(0, ifstr.beg);

                if (length > 0)
                {
                    data = new char[length];
                    ifstr.read(data, length);
                }
            }

            if (data != nullptr)
            {
                rc.GetBuffer().SetData(length, reinterpret_cast<u8*>(data));
            }

            delete[] data;
        }

        rc&(*sceneScenario);

        FRequestLoad = false;
        FRequestQuickLoad = false;
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