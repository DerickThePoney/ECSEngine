#pragma once
#include "Common/MemoryView.h"

namespace ECSEngine
{
namespace Rendering
{
enum OperatorMask
{
    NONE = 0,
    APPLY_ON_MESH = 1,
};

class VisualModel;
class Carrier;
class AbstractGFXOperatorDescriptor
{
public:
    AbstractGFXOperatorDescriptor(const char* parName)
        : FName(parName)
    {
    }

    void DrawInEditor();

    void ApplyChangesOnMesh(VisualModel& parModel, const Carrier& parCarrier){};

    virtual OperatorMask GetMask() const = 0;

    const char* Name() const { return FName; }

protected:
    virtual void VirtualDrawInEditor(){};

private:
    const char* FName = nullptr;
};

namespace GFXOperatorDescriptorFactory
{
void DestroyManager();
bool RegisterOperatorDescriptor(const char* parOperatorName, const AbstractGFXOperatorDescriptor* (*parFactory)());
std::unique_ptr<const AbstractGFXOperatorDescriptor> CreateOperator(const char* parOperatorName);
MemoryView<const char*> GetOperatorsList();
} // namespace GFXOperatorDescriptorFactory

#define REGISTER_OPERATOR_FACTORY(TYPE)                                                                                                                                            \
    const AbstractGFXOperatorDescriptor* Create##TYPE() { return new TYPE(#TYPE); }                                                                                                \
    static const bool s_Registered_##TYPE = GFXOperatorDescriptorFactory::RegisterOperatorDescriptor(#TYPE, &Create##TYPE);

} // namespace Rendering
} // namespace ECSEngine
