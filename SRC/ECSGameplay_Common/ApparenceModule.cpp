#include "stdafx.h"

#include "ApparenceModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "RenderingCore/GFXRepresentationDescriptor.h"
#include "RenderingCore/GFXRepresentationDescriptorManager.h"
#include "RenderingCore/GFXRepresentationInitialiser.h"
#include "RenderingCore/GFXRepresentationProxy.h"

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
    auto descriptor = Rendering::GFXRepresentationDescriptorManager::Instance().Decriptors();
#ifdef ENABLE_SECURITY_CHECKS
    if (!FGFXRepresentationDescriptorName.empty())
    {
        bool found = false;
        foreachitemconst(d, descriptor)
        {
            if (d->Name() == FGFXRepresentationDescriptorName)
            {
                found = true;
                break;
            }
        }
        AssertRelease(found);
    }
#endif

    if (ImGui::BeginCombo("GFXRep", FGFXRepresentationDescriptorName.c_str()))
    {
        foreachitemconst(d, descriptor)
        {
            if (ImGui::Selectable(d->Name().c_str(), d->Name() == FGFXRepresentationDescriptorName))
            {
                FGFXRepresentationDescriptorName = d->Name();
            }
        }
        ImGui::EndCombo();
    }

    EDITOR_PROPERTY_BOOL("Is selectable", FIsSelectable);
}

ApparenceModule::ApparenceModule()
    : Module()
{
}

void ApparenceModule::VirtualInit(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters)
{
    parent_type::VirtualInit(parUnitId, parParameters);

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
    AssertRelease(!Template<ApparenceModuleTemplate>()->GFXRepresentationDescriptorName().empty());
    init.FRepresentationDescriptor = Template<ApparenceModuleTemplate>()->GFXRepresentationDescriptorName();
    init.HasVisuals = true;
    init.FIsSelectable = { true, true };

    FProxy->Initialise(init);
}

void ApparenceModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();
    delete FProxy;
}

} // namespace ECSEngine
