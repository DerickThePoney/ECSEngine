#pragma once

#include "Common/SaveLoadData.h"

#include <fstream>

namespace ECSEngine
{
template<typename T>
void SaveLoadManager::HandleSaveLoad(T* sceneScenario)
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
        std::string filename = (FRequestLoad) ? FSaveFilename : "QuickSave.sav";
        SavingSystem::ReadChunk rc;

        {
            char* data = nullptr;
            u32 length = 0;

            std::ifstream ifstr(FSaveFolder + filename, std::ios::binary);
            AssertRelease(ifstr.good());
            if (ifstr.good())
            {
                ifstr.seekg(0, ifstr.end);
                length = static_cast<u32>(ifstr.tellg());
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
} // namespace ECSEngine
