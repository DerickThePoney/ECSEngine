#pragma once
#include "Common/Polygon.h"
#include "Common/PoolAllocator.h"
#include "Common/Triangle.h"
#include "SceneActions.h"

namespace ECSEngine
{
/*************************************************************/
/*            SceneActionWithBaseSceneItem                   */
/*************************************************************/

class BaseSceneItem;
class SceneActionWithBaseSceneItem : public ISceneAction
{
public:
    SceneActionWithBaseSceneItem(const std::string& parFName = "Dummy");
    virtual ~SceneActionWithBaseSceneItem();

    const BaseSceneItem* GetSceneItem() const { return FSceneItem; }

    void SetSceneItem(const BaseSceneItem* parSceneItem);

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualDrawEditor() override;

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<ISceneAction>(this), PROPERTY(SceneItemID));
    }

protected:
    const BaseSceneItem* FSceneItem = nullptr;
    u32 FSceneItemID = -1;
};

/*************************************************************/
/*            SceneActionCreateMainCamera                    */
/*************************************************************/
class SceneActionCreateMainCamera : public SceneActionWithBaseSceneItem
{
    DECLARE_SCENE_ACTION(SceneActionCreateMainCamera);

public:
    SceneActionCreateMainCamera(const std::string& parFName = "Dummy");
    virtual ~SceneActionCreateMainCamera();

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial) override;

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<SceneActionWithBaseSceneItem>(this), PROPERTY(CameraName), PROPERTY(Fov), PROPERTY(NearPlane), PROPERTY(FarPlane));
    }

private:
    std::string FCameraName = "GameplayCamera";
    float FFov = 60.0f;
    float FNearPlane = 0.1f;
    float FFarPlane = 100.0f;
};

/*************************************************************/
/*            SceneActionPolygonalPattern                    */
/*************************************************************/
class SceneActionPolygonalPattern : public SceneActionWithBaseSceneItem
{
    DECLARE_SCENE_ACTION(SceneActionPolygonalPattern);

public:
    SceneActionPolygonalPattern(const std::string& parFName = "Dummy");
    virtual ~SceneActionPolygonalPattern();

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualStart() override;

    virtual void VirtualDrawEditor() override;

    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial) override;

private:
    void ComputeTriangulation();

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<SceneActionWithBaseSceneItem>(this));
        Polygon2D defaultPolygon;
        defaultPolygon.append(std::vector<glm::vec2>({ glm::vec2(0.f, 0.f), glm::vec2(1.f, 0.f), glm::vec2(0.f, 1.f) }));
        PROPERTYFIELD(Polygon, defaultPolygon);
    }

private:
    Polygon2D FPolygon;
    std::vector<Triangle2D> FTriangles;
    std::vector<glm::vec3> vertices;
    std::vector<std::vector<glm::vec3>> verticesForTriangles;
};

} // namespace ECSEngine