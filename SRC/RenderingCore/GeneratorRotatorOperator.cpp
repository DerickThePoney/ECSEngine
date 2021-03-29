#include "stdafx.h"

#include "GFXOperator.h"

namespace ECSEngine
{
namespace Rendering
{
class GeneratorRotatorOperatorDescriptor : public AbstractGFXOperatorDescriptor
{
public:
    GeneratorRotatorOperatorDescriptor(const char* parName)
        : AbstractGFXOperatorDescriptor(parName)
    {
    }

    OperatorMask GetMask() const override { return OperatorMask::APPLY_ON_MESH; }
};

REGISTER_OPERATOR_FACTORY(GeneratorRotatorOperatorDescriptor);
} // namespace Rendering
} // namespace ECSEngine
