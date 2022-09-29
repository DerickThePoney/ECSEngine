#include "stdafx.h"

#include "ApparenceModule.h"

#include "Application/PropertyDrawer.h"
#include "Common/SavingSystemImplementation.h"
#include "Common/TimeManager.h"
#include "ECSCore/EntityTemplateManagerMethods.h"
#include "ECSCore/ModuleAccessor.h"
#include "ECSCore/ModuleParameters.h"
#include "ECSCore/ModuleUtils.h"
#include "OrientationModule.h"
#include "PositionModule.h"
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

    vec3 pos = parParameters.Get_IFP<ModuleParameters::Position>(vec3(0.0f));
    quat orient(1.0f, 0.0f, 0.0f, 0.0f);
    
    if (parParameters.HasParameter<ModuleParameters::EulerAnglesXYZ>())
    {
        const vec3 euler = parParameters.Get_IFP<ModuleParameters::EulerAnglesXYZ>(vec3(0.0f));
        orient = quat::FromMat4(EulerAnglesXYZ(euler.x, euler.y, euler.z));
    }
    else if (parParameters.HasParameter<ModuleParameters::Orientation>())
    {
        orient = parParameters.Get_IFP<ModuleParameters::Orientation>(quat());
    }

    InitGFXProxy(pos, orient);
}

void ApparenceModule::VirtualDeinit()
{
    parent_type::VirtualDeinit();
    delete FProxy;
}

void ApparenceModule::VirtualOnLoaded()
{
    parent_type::VirtualOnLoaded();

    ManualLockModuleAccessor<PositionModule> positionAccessor(UnitId().GetWorld());
    ManualLockModuleAccessor<OrientationModule> orientationAccessor(UnitId().GetWorld());
    positionAccessor.LockIFN();
    orientationAccessor.LockIFN();
    const PositionModule* positionModule = positionAccessor[UnitId()];
    AlwaysCheckedAssert(positionModule != nullptr);
    const OrientationModule* orientationModule = orientationAccessor[UnitId()];
    AssertRelease(orientationModule != nullptr);

    positionAccessor.UnlockIFN();
    orientationAccessor.UnlockIFN();

    InitGFXProxy(positionModule->GetPosition3D(), orientationModule->GetOrientation());
}

void ApparenceModule::InitGFXProxy(const vec3& parPosition, const quat& parOrientation)
{
    Rendering::GFXRepresentationInitialiser init;
    init.FPosition = parPosition;
    init.FOrientation = parOrientation;
    init.HasCarier = true;
    init.FCurrentTime = TimeManager::FrameStartTime();
    AssertRelease(!Template<ApparenceModuleTemplate>()->GFXRepresentationDescriptorName().empty());
    init.FRepresentationDescriptor = Template<ApparenceModuleTemplate>()->GFXRepresentationDescriptorName();
    init.HasVisuals = true;
    init.FIsSelectable = { true, true };

    FProxy = new Rendering::GFXRepresentationProxy();
    FProxy->Initialise(init);
}

IMPLEMENT_SAVELOAD_ABILITIES(ApparenceModule);
template<typename Chunk, bool isWriting>
void ApparenceModule::SaveLoad(Chunk& parChunk)
{
    parent_type::SaveLoad(parChunk);
}
} // namespace ECSEngine
