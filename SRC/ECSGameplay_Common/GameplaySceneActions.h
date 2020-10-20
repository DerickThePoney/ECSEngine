#pragma once
#include "Application/ApplicationSceneActions.h"
#include "Common/PoolAllocator.h"

namespace ECSEngine
{
/*************************************************************/
/*            SpawnEntitySceneAction                         */
/*************************************************************/

class EntityTemplate;
class SpawnEntitySceneAction : public SceneActionWithBaseSceneItem
{
    DECLARE_SCENE_ACTION(SpawnEntitySceneAction, SceneActionWithBaseSceneItem);

public:
    SpawnEntitySceneAction(const std::string& parFName = "Dummy");
    virtual ~SpawnEntitySceneAction();

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;
    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

public:
    const std::string& GetEntityTemplateName() const { return FEntityTemplateName; }

    void SetEntityTemplateName(const std::string& parName) { FEntityTemplateName = parName; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this), PROPERTY(EntityTemplateName));
    }

private:
    std::string FEntityTemplateName = "Entity template name";
    const EntityTemplate* FTemplate = nullptr;
};

/*************************************************************/
/*        SpawnEntitiesInPolygonalPatternSceneAction         */
/*************************************************************/
class SpawnEntitiesInPolygonalPatternSceneAction : public SceneActionPolygonalPattern
{
    DECLARE_SCENE_ACTION(SpawnEntitiesInPolygonalPatternSceneAction, SceneActionPolygonalPattern);

public:
    SpawnEntitiesInPolygonalPatternSceneAction(const std::string& parFName = "Dummy");
    virtual ~SpawnEntitiesInPolygonalPatternSceneAction();

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;
    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

public:
    const std::string& GetEntityTemplateName() const { return FEntityTemplateName; }

    void SetEntityTemplateName(const std::string& parName) { FEntityTemplateName = parName; }

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this), PROPERTY(EntityTemplateName));
    }

private:
    std::string FEntityTemplateName = "Entity template name";
    const EntityTemplate* FTemplate = nullptr;
};
} // namespace ECSEngine