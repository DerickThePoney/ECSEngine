#pragma once
#include "SceneActions.h"
#include "SceneItems.h"

namespace ECSEngine
{
class Scene
{
public:
    Scene();
    ~Scene();

    const std::string& GetName() const { return FName; }
    void SetName(const std::string& parName) { FName = parName; }

    const size_t& GetSceneId() const { return std::hash<std::string>().operator()(FName); } // FNV 1a

    void AddSceneItem(const u32 parSceneItemTypeId);
    void RemoveSceneItem(const u32 parId);

    void Initialise();
    void Destroy();

    void OnDrawEditor();
    void Update();
    void Render();

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(SceneItems));
    }

private:
    std::string FName;

    std::map<u32, std::vector<std::shared_ptr<BaseSceneItem>>> FSceneItems;

    std::vector<ISceneAction*> FActions;
    u32 FCurrentAction = 0;
};
} // namespace ECSEngine