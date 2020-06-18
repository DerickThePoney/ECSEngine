#pragma once
#include "Common/IdGenerator.h"
#include "SceneActions.h"
#include "SceneItems.h"

namespace ECSEngine
{
class Scene
{
public:
    Scene();
    virtual ~Scene();

    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    const size_t GetSceneId() const { return std::hash<std::string>().operator()(FName); } // FNV 1a

    void AddSceneItem(const u32 parSceneItemTypeId);
    void RemoveSceneItem(const std::vector<std::shared_ptr<BaseSceneItem>>::iterator parWhere);

    virtual void Initialise();
    virtual void Destroy();

    virtual void Update();
    virtual void Render();

    void SetItemHovered(const u32 parId);
    void SetItemSelected(const u32 parId);

    const std::vector<std::shared_ptr<BaseSceneItem>>& GetSceneItems() const { return FSceneItems; }
    const std::vector<std::shared_ptr<ISceneAction>>& GetSceneActions() const { return FActions; }

    std::vector<std::shared_ptr<BaseSceneItem>>& GetSceneItemsForWriting() { return FSceneItems; }
    std::vector<std::shared_ptr<ISceneAction>>& GetSceneActionsForWriting() { return FActions; }

    void AddSceneActionStealOwnership(ISceneAction* parAction);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name));
        ar(PROPERTY(SceneItemsIdGenerator));
        ar(PROPERTY(SceneItems));
        ar(PROPERTY(Actions));

        foreachitem(action, FActions) action->SetScene(this);
    }

private:
    std::string FName;

    IdGenerator FSceneItemsIdGenerator;
    std::vector<std::shared_ptr<BaseSceneItem>> FSceneItems;

    std::vector<std::shared_ptr<ISceneAction>> FActions;
    u32 FCurrentAction = 0;
};
} // namespace ECSEngine
