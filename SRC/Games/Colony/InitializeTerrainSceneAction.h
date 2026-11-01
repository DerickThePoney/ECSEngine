#pragma once
#include "Application/SceneActions.h"
#include "Rendering/TerrainDescriptor.h"

namespace ECSEngine
{
class InitializeTerrainSceneAction : public ISceneAction
{
    DECLARE_SCENE_ACTION(InitializeTerrainSceneAction, ISceneAction);

public:
    InitializeTerrainSceneAction(const std::string& parName = "Dummy");
    virtual ~InitializeTerrainSceneAction() { }

protected:
    virtual void VirtualInitialise(const SceneScenario* parScene) override;

    virtual void VirtualShutdown() override;

    virtual void VirtualDrawEditor() override;

    virtual bool VirtualDrawInSceneEditor(Rendering::DrawCommandBuffer& parCommandBuffer, Rendering::MaterialInstanceHandle& parMaterial) override;

    virtual void VirtualStart() override;

public:
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(cereal::base_class<parent_type>(this));
        PROPERTYFIELD(TerrainDescriptor, Rendering::TerrainDescriptor());
    }

private:
    Rendering::TerrainDescriptor FTerrainDescriptor;
};
} // namespace ECSEngine
