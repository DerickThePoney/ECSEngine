#pragma once
#include "Application/ApplicationSceneActions.h"
#include "Common/NavMesh.h"
#include "Common/NavMeshPath.h"
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
        PROPERTYFIELD(NumberOfEntities, 500);
    }

protected:
    void GenerateRandomPoints();

private:
    std::string FEntityTemplateName = "Entity template name";
    const EntityTemplate* FTemplate = nullptr;

    std::vector<vec2> FRandomPoints;

    u32 FNumberOfEntities = 1000;

    bool FShowEntitiesInEditor = true;
};

/*************************************************************/
/*        ConvexHullTestsPolygonalPatternSceneAction         */
/*************************************************************/
class ConvexHullTestsPolygonalPatternSceneAction : public SceneActionPolygonalPattern
{
    DECLARE_SCENE_ACTION(ConvexHullTestsPolygonalPatternSceneAction, SceneActionPolygonalPattern);

public:
    ConvexHullTestsPolygonalPatternSceneAction(const std::string& parFName = "Dummy");
    virtual ~ConvexHullTestsPolygonalPatternSceneAction();

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;
    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this));
        PROPERTYFIELD(NumberOfEntities, 500);
    }

protected:
    void GeneratePoints();
    void GenerateHull();

private:
    std::vector<vec2> FRandomPoints;
    std::vector<vec3> FPolygonVertices;

    u32 FNumberOfEntities = 1000;

    bool FShowEntitiesInEditor = true;

    float FTimeTaken = 0.f;
    Polygon2D FConvexHull;
};

/*******************************************/
/*        CreateNavMeshSceneAction         */
/*******************************************/
class CreateNavMeshSceneAction : public SceneActionPolygonalPattern
{
    DECLARE_SCENE_ACTION(CreateNavMeshSceneAction, SceneActionPolygonalPattern);

public:
    CreateNavMeshSceneAction(const std::string& parFName = "Dummy");
    virtual ~CreateNavMeshSceneAction() { }

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;
    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial);

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this));
    }
};
} // namespace ECSEngine
