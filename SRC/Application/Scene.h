#pragma once
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

    const size_t& GetSceneId() const { return std::hash<std::string>().operator()(FName); } // FNV 1a

    void AddSceneItem(const u32 parSceneItemTypeId);
    void RemoveSceneItem(const u32 parId);

    virtual void Initialise();
    virtual void Destroy();

    virtual void Update();
    virtual void Render();

    std::vector<std::shared_ptr<BaseSceneItem>>& GetSceneItems() { return FSceneItems; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name));
        ar(PROPERTY(SceneItems));
    }

private:
    std::string FName;

    std::vector<std::shared_ptr<BaseSceneItem>> FSceneItems;

    std::vector<std::unique_ptr<ISceneAction>> FActions;
    u32 FCurrentAction = 0;
};
} // namespace ECSEngine