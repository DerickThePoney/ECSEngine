#include "stdafx.h"

#include "Common/PoolAllocator.h"
#include "GFXOperator.h"

namespace ECSEngine
{
namespace Rendering
{
class GeneratorRotatorOperatorDescriptor : public AbstractGFXOperatorDescriptor
{
    DECLARE_POOL_ALLOCATED(GeneratorRotatorOperatorDescriptor);

public:
    GeneratorRotatorOperatorDescriptor(const char* parName)
        : AbstractGFXOperatorDescriptor(parName)
    {
    }

    OperatorMask GetMask() const override { return OperatorMask::APPLY_ON_MESH; }
};

IMPLEMENT_POOL_ALLOCATED(GeneratorRotatorOperatorDescriptor);

REGISTER_OPERATOR_FACTORY(GeneratorRotatorOperatorDescriptor);
} // namespace Rendering
} // namespace ECSEngine
