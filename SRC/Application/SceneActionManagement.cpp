#include "stdafx.h"

#include "SceneActionManagement.h"

#include "ApplicationSceneActions.h"
#include "Common/Singleton.h"
#include "ECSGameplay_Common/GameplaySceneActions.h"

namespace ECSEngine
{
class SceneActionsFactoryManager : public Singleton<SceneActionsFactoryManager>
{
public:
    SceneActionsFactoryManager()
        : Singleton()
    {
    }

    ~SceneActionsFactoryManager() {}

    bool RegisterSceneActionFactory(const u32 parId, ISceneAction* (*parFactory)());
    ISceneAction* CreateSceneAction(const u32 parId);
    const std::map<u32, std::string>& GetSceneActionsList() { return FSceneActionsList; }

private:
    std::unordered_map<u32, ISceneAction* (*)()> FSceneActionsFactories;
    std::map<u32, std::string> FSceneActionsList;
};

bool SceneActionsFactoryManager::RegisterSceneActionFactory(const u32 parId, ISceneAction* (*parFactory)())
{
    AssertRelease(FSceneActionsFactories.find(parId) == FSceneActionsFactories.end());
    FSceneActionsFactories[parId] = parFactory;
    ISceneAction* action = CreateSceneAction(parId);
    FSceneActionsList[parId] = action->GetTypeName();
    delete action;
    return true;
}

ISceneAction* SceneActionsFactoryManager::CreateSceneAction(const u32 parId)
{
    AlwaysCheckedAssert(FSceneActionsFactories.find(parId) != FSceneActionsFactories.end());
    ISceneAction* temp = FSceneActionsFactories[parId]();
    AssertRelease(temp != nullptr);
    return temp;
}

namespace SceneActionManagement
{

void InitialiseFactory()
{
#ifdef DECLARE_SCENE_ACTION
#define DECLARE_SCENE_ACTION_RECOVER DECLARE_SCENE_ACTION
#undef DECLARE_SCENE_ACTION
#endif

#define DECLARE_SCENE_ACTION(TYPE)                                                                                                                                                 \
    TYPE* t##TYPE = new TYPE;                                                                                                                                                      \
    delete t##TYPE;
#include "SceneActionIds.inl"
#undef DECLARE_SCENE_ACTION

#ifdef DECLARE_SCENE_ACTION_RECOVER
#define DECLARE_SCENE_ACTION DECLARE_SCENE_ACTION_RECOVER
#undef DECLARE_SCENE_ACTION_RECOVER
#endif
}

bool RegisterSceneActionFactory(const u32 parId, ISceneAction* (*parFactory)())
{
    if (!SceneActionsFactoryManager::HasInstance())
        SceneActionsFactoryManager::CreateIFP();

    AssertRelease(SceneActionsFactoryManager::HasInstance());
    return SceneActionsFactoryManager::Instance().RegisterSceneActionFactory(parId, parFactory);
}

ISceneAction* CreateSceneAction(const u32 parId)
{
    AssertRelease(SceneActionsFactoryManager::HasInstance());
    return SceneActionsFactoryManager::Instance().CreateSceneAction(parId);
}

const std::map<u32, std::string>& GetSceneActionsList()
{
    AssertRelease(SceneActionsFactoryManager::HasInstance());
    return SceneActionsFactoryManager::Instance().GetSceneActionsList();
}

} // namespace SceneActionManagement
} // namespace ECSEngine