#include "stdafx.h"

#include "Common/PoolAllocator.h"
#include "Common/TimeManager.h"
#include "GFXMessage.h"
#include "GFXOperator.h"
#include "Skeletton.h"

namespace ECSEngine
{
namespace Rendering
{
class GeneratorRotatorOperatorDescriptor : public AbstractGFXOperatorDescriptor
{
    DECLARE_POOL_ALLOCATED(GeneratorRotatorOperatorDescriptor);

public:
    GeneratorRotatorOperatorDescriptor()
        : AbstractGFXOperatorDescriptor("GeneratorRotatorOperatorDescriptor")
    {
    }
    virtual ~GeneratorRotatorOperatorDescriptor() = default;

    virtual AbstractGFXOperator* CreateOperator() const override;

    float RotationSpeed() const { return FRotationSpeed; }

    SERIALIZE() { PROPERTYFIELD(RotationSpeed, 0.f); }

protected:
    virtual void VirtualDrawInEditor() override;

private:
    float FRotationSpeed = 0.f;
};

IMPLEMENT_POOL_ALLOCATED(GeneratorRotatorOperatorDescriptor);

void GeneratorRotatorOperatorDescriptor::VirtualDrawInEditor()
{
    float rotationTRS = 0.5f * FRotationSpeed / Pi();
    ImGui::InputFloat("Rotation speed (TRS)", &rotationTRS, 1.f, 10.f);
    FRotationSpeed = 2.f * Pi() * rotationTRS;
}

REGISTER_OPERATOR_FACTORY(GeneratorRotatorOperatorDescriptor);

class GeneratorRotatorOperator : public AbstractGFXOperator
{
    DECLARE_POOL_ALLOCATED(GeneratorRotatorOperator);

public:
    GeneratorRotatorOperator(const GeneratorRotatorOperatorDescriptor* parDescriptor)
        : FDescriptor(parDescriptor)
    {
        AssertRelease(FDescriptor != nullptr);
    }

    virtual ~GeneratorRotatorOperator() = default;

    virtual OperatorMask GetMask() const override { return OperatorMask::APPLY_ON_MESH; }

    virtual void ApplyChangesOnMesh(const GFXMessage& parMessages, VisualModel* parModel, SkelettonPose* parSkelettonPose, const Carrier* parCarrier) override;

private:
    const GeneratorRotatorOperatorDescriptor* FDescriptor = nullptr;
    u8 FSkelettonJoint = 0xFF;
};

AbstractGFXOperator* GeneratorRotatorOperatorDescriptor::CreateOperator() const
{
    return new GeneratorRotatorOperator(this);
}

void GeneratorRotatorOperator::ApplyChangesOnMesh(const GFXMessage& parMessages, VisualModel* parModel, SkelettonPose* parSkelettonPose, const Carrier* parCarrier)
{
    AssertRelease(parSkelettonPose != nullptr);
    if (FSkelettonJoint == 0xFF)
    {
        FSkelettonJoint = parSkelettonPose->GetSkeletton()->FindSkelettonJoint("RotatingPale");
        AlwaysCheckedAssert(FSkelettonJoint != 0xFF);
        if (FSkelettonJoint == 0xFF)
            return;
    }

    mat4* localPoses = parSkelettonPose->LocalPoses();
    AssertRelease(localPoses != nullptr);
    const float angle = FDescriptor->RotationSpeed() * TimeManager::FrameDeltaTime();
    localPoses[FSkelettonJoint] = Rotation(angle, vec3(0.f, 0.f, 1.f)) * localPoses[FSkelettonJoint];
    parSkelettonPose->SetDirty();
}

IMPLEMENT_POOL_ALLOCATED(GeneratorRotatorOperator);

} // namespace Rendering
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::Rendering::GeneratorRotatorOperatorDescriptor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::Rendering::AbstractGFXOperatorDescriptor, ECSEngine::Rendering::GeneratorRotatorOperatorDescriptor)
