#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
class GameScenarioUpdater;
class SaveLoadManager : public Singleton<SaveLoadManager>
{
public:
    void Initialise();
    void Cleanup();

    void GetSaveFilesList(std::vector<std::string>& outSaveFilesList);

    void Update();
    void HandleSaveLoad(GameScenarioUpdater* sceneScenario);

    void RequestSave(const std::string& parFilename);
    void RequestLoad(const std::string& parFilename);

    void RequestQuickSave();
    void RequestQuickLoad();

private:
    std::string FSaveFolder;

    bool FRequestSave = false;
    bool FRequestLoad = false;
    std::string FSaveFilename;

    bool FRequestAutoSave = false;
    float FAutoSaveTimer = std::numeric_limits<float>::max();

    bool FRequestQuickSave = false;
    bool FRequestQuickLoad = false;
};
} // namespace ECSEngine