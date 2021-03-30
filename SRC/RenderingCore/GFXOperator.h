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
class AbstractGFXOperator;
class AbstractGFXOperatorDescriptor
{
public:
    AbstractGFXOperatorDescriptor(const char* parName)
        : FName(parName)
    {
    }

    void DrawInEditor();

    const char* Name() const { return FName; }

    virtual AbstractGFXOperator* CreateOperator() const = 0;

protected:
    virtual void VirtualDrawInEditor(){};

protected:
    const char* FName = nullptr;
};

class AbstractGFXOperator
{
public:
    virtual void ApplyChangesOnMesh(VisualModel& parModel, const Carrier& parCarrier){};

    virtual OperatorMask GetMask() const = 0;
};

namespace GFXOperatorDescriptorFactory
{
void DestroyManager();
bool RegisterOperatorDescriptor(const char* parOperatorName, const AbstractGFXOperatorDescriptor* (*parFactory)());
std::unique_ptr<const AbstractGFXOperatorDescriptor> CreateOperator(const char* parOperatorName);
MemoryView<const char*> GetOperatorsList();
} // namespace GFXOperatorDescriptorFactory

#define REGISTER_OPERATOR_FACTORY(TYPE)                                                                                                                                            \
    const AbstractGFXOperatorDescriptor* Create##TYPE() { return new TYPE(); }                                                                                                     \
    static const bool s_Registered_##TYPE = GFXOperatorDescriptorFactory::RegisterOperatorDescriptor(#TYPE, &Create##TYPE);

} // namespace Rendering
} // namespace ECSEngine
