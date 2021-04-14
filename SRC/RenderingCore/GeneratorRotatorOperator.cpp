#include "stdafx.h"

#include "Common/PoolAllocator.h"
#include "GFXMessage.h"
#include "GFXOperator.h"

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

    SERIALIZE() {}

protected:
    virtual void VirtualDrawInEditor() override;

private:
    float FRotationSpeed = 0.f;
};

IMPLEMENT_POOL_ALLOCATED(GeneratorRotatorOperatorDescriptor);

void GeneratorRotatorOperatorDescriptor::VirtualDrawInEditor()
{
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
};

AbstractGFXOperator* GeneratorRotatorOperatorDescriptor::CreateOperator() const
{
    return new GeneratorRotatorOperator(this);
}

void GeneratorRotatorOperator::ApplyChangesOnMesh(const GFXMessage& parMessages, VisualModel* parModel, SkelettonPose* parSkelettonPose, const Carrier* parCarrier)
{
}

IMPLEMENT_POOL_ALLOCATED(GeneratorRotatorOperator);

} // namespace Rendering
} // namespace ECSEngine

CEREAL_REGISTER_TYPE(ECSEngine::Rendering::GeneratorRotatorOperatorDescriptor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::Rendering::AbstractGFXOperatorDescriptor, ECSEngine::Rendering::GeneratorRotatorOperatorDescriptor)
