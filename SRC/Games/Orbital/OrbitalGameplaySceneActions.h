#pragma once
#include "ECSGameplay_Common/GameplaySceneActions.h"

namespace ECSEngine
{
class CreateSolarSystemSceneAction : public SceneActionWithBaseSceneItem
{
    DECLARE_SCENE_ACTION(CreateSolarSystemSceneAction, SceneActionWithBaseSceneItem);

public:
    CreateSolarSystemSceneAction(const std::string& parName = "Dummy");
    virtual ~CreateSolarSystemSceneAction() { }

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualDrawEditor() override;

    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial) override;

    virtual void VirtualStart() override;

private:
    void DrawTemplate(const EntityTemplate* Template,
          const vec3 Offset,
          const BaseSceneItem* Item,
          Rendering::DrawCommandBuffer& parCommandBuffer,
          Rendering::MaterialInstanceHandle& parMaterial) const;

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this));
        PROPERTYFIELD(PlanetTemplateName, "");
        PROPERTYFIELD(MoonTemplateName, "");
    }

private:
    std::string FPlanetTemplateName = "Planet template name";
    std::string FMoonTemplateName = "Moon template name";
    const EntityTemplate* FPlanetTemplate = nullptr;
    const EntityTemplate* FMoonTemplate = nullptr;
};
} // namespace ECSEngine
