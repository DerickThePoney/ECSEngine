#include "stdafx.h"

#include "ApparenceModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/Resource.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityTemplateManager.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "RenderingCore/Carrier.h"
#include "RenderingCore/MaterialManager.h"
#include "RenderingCore/MeshManager.h"

CEREAL_REGISTER_TYPE(ECSEngine::ApparenceModuleTemplate);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ModuleTemplate, ECSEngine::ApparenceModuleTemplate)
namespace ECSEngine
{
IMPLEMENT_MODULE_TEMPLATE(ApparenceModule, ApparenceModuleTemplate);

Module* ApparenceModuleTemplate::CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
{
    return NewModule<ApparenceModule>(this, parUnitId, parParameters);
}

void ApparenceModuleTemplate::VirtualDrawEditor()
{
    EDITOR_PROPERTY_STRING("MeshFile", FMeshFileName, true, "*.fbx.gen");
    EDITOR_PROPERTY_STRING("MaterialFile", FMaterialFileName, true, "*.material");
}

ApparenceModule::ApparenceModule()
    : Module()
{
}

void ApparenceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);
    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(Resource(Template<ApparenceModuleTemplate>()->GetMeshFileName()));
    FMaterialHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(Template<ApparenceModuleTemplate>()->GetMaterialFileName());

    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialHandle.IsValid());

    const ApparenceModuleTemplate* temp = Template<ApparenceModuleTemplate>();

    FProxy = new Rendering::GFXRepresentationProxy();

    glm::vec3 pos = parParameters.Get_IFP<ModuleParameters::Position>(glm::vec3(0.0f));
    glm::quat orient(1.0f, 0.0f, 0.0f, 0.0f);
    if (parParameters.HasParameter<ModuleParameters::YawPitchRoll>())
    {
        const glm::vec3 yawPitchRoll = parParameters.Get_IFP<ModuleParameters::YawPitchRoll>(glm::vec3(0.0f));
        orient = glm::quat(yawPitchRoll);
    }
    else if (parParameters.HasParameter<ModuleParameters::EulerAngles>())
    {
        const glm::vec3 euler = parParameters.Get_IFP<ModuleParameters::EulerAngles>(glm::vec3(0.0f));
        orient = glm::quat_cast(glm::eulerAngleXYZ(euler.x, euler.y, euler.z));
    }
    else if (parParameters.HasParameter<ModuleParameters::Orientation>())
    {
        orient = parParameters.Get_IFP<ModuleParameters::Orientation>(glm::quat());
    }

    Rendering::GFXRepresentationInitialiser init;
    init.FPosition = pos;
    init.FOrientation = orient;
    init.HasCarier = true;
    init.FCurrentTime = TimeManager::FrameStartTime();

    FProxy->Initialise(UnitId(), init);
}

void ApparenceModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();
    delete FProxy;
}

const Rendering::MeshHandle& ApparenceModule::GetMeshHandle() const
{
    return FMeshHandle;
}

const Rendering::MaterialInstanceHandle& ApparenceModule::GetMaterialHandle() const
{
    return FMaterialHandle;
}

} // namespace ECSEngine